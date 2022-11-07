#include "stdafx.h"
#include "..\Public\Explosion.h"
#include "GameInstance.h"
CExplosion::CExplosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CExplosion::CExplosion(const CExplosion & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExplosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CExplosion::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "CExplosion");
	m_Tag = L"Explosion";

	m_pTransformCom->Set_Scale(XMVectorSet(0.02f, 0.02f, 0.02f, 1.f));

	_float3 vPos;
	memcpy(&vPos, pArg, sizeof(_float3));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	return S_OK;
}

_bool CExplosion::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return false;
	if (m_bDestroy)
		return true;

	m_fCurrentTime += fTimeDelta;
	//제한시간을 넘었다면
	if (m_fCurrentTime >= m_fDuration)
	{
		m_bDissolve = true;
	}
	if (m_bDissolve)
	{
		m_fCut += fTimeDelta* m_fDissolveSpeed;
		m_iPass = 4;
	}

	m_vScale.x += 0.01f * fTimeDelta * m_fExplosionSpeed;
	m_vScale.y += 0.01f * fTimeDelta * m_fExplosionSpeed;
	m_vScale.z += 0.01f * fTimeDelta * m_fExplosionSpeed;

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

	if (m_fCut >= 1.f)
	{
		return true;
	}

	return false;
}

void CExplosion::LateTick(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	if (m_bDestroy)
		return;
	if (nullptr == m_pRendererCom)
		return;

	m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif
}

HRESULT CExplosion::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_Cut", &m_fCut, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPass)))
			return E_FAIL;
	}

	return S_OK;
}

void CExplosion::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CExplosion::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CExplosion::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CExplosion::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Explosion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BurnNoise"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CExplosion * CExplosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExplosion*		pInstance = new CExplosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CExplosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExplosion::Clone(void * pArg)
{
	CExplosion*		pInstance = new CExplosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CExplosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExplosion::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
}
