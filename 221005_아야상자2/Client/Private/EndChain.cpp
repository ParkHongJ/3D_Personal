#include "stdafx.h"
#include "..\Public\EndChain.h"
#include "GameInstance.h"
#include "Sword.h"
#include "Ras_Samrah.h"
#include "ChaudronChain.h"
CEndChain::CEndChain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEndChain::CEndChain(const CEndChain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEndChain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEndChain::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -0.5f, 1.f));
	strcpy_s(m_szName, "EndChain");
	m_Tag = L"EndChain";
	m_bEnable = false;

	m_eChain = CEndChain::NORMAL;
	return S_OK;
}

_bool CEndChain::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pParentTransformCom->Get_WorldMatrix());
	return false;
}

void CEndChain::LateTick(_float fTimeDelta)
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

HRESULT CEndChain::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_vChainColor", &m_vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);



	_uint		iNumMeshes = m_pModelCom[m_eChain]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom[m_eChain]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_eChain]->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_eChain]->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEndChain::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CEndChain::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	if (pOther->CompareTag(L"Player_Sword"))
	{
		_float fDamage = ((CSword*)pOther)->GetDamage();

		if (nullptr == m_pRasSamrah)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pRasSamrah = (CRas_Samrah*)((CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Monster", L"Com_Transform", 0)->GetOwner());
			Safe_AddRef(m_pRasSamrah);
			RELEASE_INSTANCE(CGameInstance);

			GetDamaged(fDamage);
			m_pRasSamrah->GetDamaged(fDamage);
		}
		else
		{
			GetDamaged(fDamage);
			m_pRasSamrah->GetDamaged(fDamage);
		}
		
	}
}

void CEndChain::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CEndChain::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CEndChain::Broken()
{
	//부서진걸로 교체하고 콜라이더를 끔.
	m_fHp = 0.f;
	m_eChain = BREAKED;
	m_bEnable = false;
	m_vColor = _float3(0.f, 0.f, 0.f);
}

void CEndChain::GetDamaged(_float fDamage)
{

	m_fHp -= fDamage;

	//보스에게도 데미지

	//최대 체력보다 낮으면
	if (m_fHp <= 0.0f)
	{
		Broken();
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CChaudronChain* pChaudronChain = (CChaudronChain*)(pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_ChaudronChain", L"Com_Transform", 0)->GetOwner());
		pChaudronChain->CheckChain();
		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CEndChain::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_EndChain"), TEXT("Com_Model"), (CComponent**)&m_pModelCom[NORMAL])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BreakedEndChain"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom[BREAKED])))
		return E_FAIL;

	CCollider::COLLIDERDESC ColliderDesc;
	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CEndChain * CEndChain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEndChain*		pInstance = new CEndChain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEndChain::Clone(void * pArg)
{
	CEndChain*		pInstance = new CEndChain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEndChain::Free()
{
	__super::Free();

	for (_uint i = 0; i < CHAIN_END; ++i)
		Safe_Release(m_pModelCom[i]);

	Safe_Release(m_pRasSamrah);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
