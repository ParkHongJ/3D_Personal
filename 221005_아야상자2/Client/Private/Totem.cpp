#include "stdafx.h"
#include "..\Public\Totem.h"
#include "GameInstance.h"
#include "Sword.h"
#include "Ras_Samrah.h"
CTotem::CTotem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTotem::CTotem(const CTotem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTotem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTotem::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 12.5f, -5.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(3.f, 0.f, 0.f, 1.f));
	strcpy_s(m_szName, "Totem");
	m_Tag = L"Totem";

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pRasSamrah = (CRas_Samrah*)((CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Monster", L"Com_Transform", 0)->GetOwner());
	Safe_AddRef(m_pRasSamrah);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool CTotem::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return false;
}

void CTotem::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_bEnable)
		m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
	if (m_bEnable)
	{
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
	}
#endif
}

HRESULT CTotem::Render()
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



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}
	return S_OK;
}

void CTotem::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	if (pOther->CompareTag(L"Player_Sword"))
	{
		GetDamaged(((CSword*)pOther)->GetDamage());
	}
}

void CTotem::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CTotem::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CTotem::GetDamaged(_float fDamage)
{
	m_fHp -= fDamage;

	//보스에게도 데미지

	//최대 체력보다 낮으면
	if (m_fHp <= 0.0f)
	{
		//부서진걸로 교체하고 콜라이더를 끔.
		m_fHp = 0.f;
		m_bEnable = false;
		m_pRasSamrah->SetNaviTypes();
	}
}

HRESULT CTotem::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Totem"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(100.f, 100.f, 100.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CTotem * CTotem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTotem*		pInstance = new CTotem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTotem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTotem::Clone(void * pArg)
{
	CTotem*		pInstance = new CTotem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTotem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTotem::Free()
{
	__super::Free();

	Safe_Release(m_pRasSamrah);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
}
