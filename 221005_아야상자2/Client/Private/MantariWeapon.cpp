#include "stdafx.h"
#include "..\Public\MantariWeapon.h"
#include "GameInstance.h"
#include "Mantari.h"

CMantariWeapon::CMantariWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMantariWeapon::CMantariWeapon(const CMantariWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMantariWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMantariWeapon::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.025f, 0.025f, 0.025f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
	strcpy_s(m_szName, "YantariWeapon");
	m_Tag = L"YantariWeapon";


	return S_OK;
}

_bool CMantariWeapon::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pParentTransformCom->Get_WorldMatrix());
	return false;
}

void CMantariWeapon::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_bEnable)
		m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif
}

HRESULT CMantariWeapon::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;

	}
	return S_OK;
}

HRESULT CMantariWeapon::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CMantariWeapon::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{

}

void CMantariWeapon::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CMantariWeapon::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CMantariWeapon::SetParry()
{
	if (nullptr == m_pMantari)
		return;

	//m_pMantari->Parried();
}

void CMantariWeapon::SetMantari(CMantari* pMantari)
{
	//sm_pMantari = pMantari;
}

HRESULT CMantariWeapon::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_YantariWeapon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.vCenter = _float3(0.f, 2.75f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CMantariWeapon * CMantariWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMantariWeapon*		pInstance = new CMantariWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMantariWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMantariWeapon::Clone(void * pArg)
{
	CMantariWeapon*		pInstance = new CMantariWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMantariWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMantariWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
