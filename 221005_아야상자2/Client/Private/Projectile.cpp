#include "stdafx.h"
#include "..\Public\Projectile.h"
#include "GameInstance.h"

CProjectile::CProjectile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pModelCom->Set_AnimIndex(0);
	//m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f,0.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f,0.f,2.f,1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((_float)(rand() % 20), 0.f, (_float)(rand() % 20), 1.f));
	_float temp = (_float)(rand() % 9);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), temp * 0.5f);
	return S_OK;
}

_bool CProjectile::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return false;
}

void CProjectile::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CProjectile::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
	//	return E_FAIL;*/


	//	if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
	//		return E_FAIL;
	//}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CProjectile::SetDir(_fvector vDir)
{
	m_pTransformCom->LookAt(vDir);
}

void CProjectile::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CProjectile::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CProjectile::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CProjectile::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;


	/* For.Com_SPHERE */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CProjectile * CProjectile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CProjectile*		pInstance = new CProjectile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CProjectile::Clone(void * pArg)
{
	CProjectile*		pInstance = new CProjectile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
