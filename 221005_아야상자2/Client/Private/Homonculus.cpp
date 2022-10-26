#include "stdafx.h"
#include "..\Public\Homonculus.h"
#include "GameInstance.h"
#include "Sword.h"
#include "Ras_Hands2.h"
CHomonculus::CHomonculus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CHomonculus::CHomonculus(const CHomonculus & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHomonculus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHomonculus::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "Homonculus");

	if (nullptr != pArg)
	{
		CRas_Hands2::SPAWN_INFO tSpawnInfo;
		memcpy(&tSpawnInfo, pArg, sizeof(CRas_Hands2::SPAWN_INFO));

		m_pNavigationCom->SetCurrentIndex(tSpawnInfo.iCellIndex);
		m_pTarget = tSpawnInfo.pTarget;
		Safe_AddRef(m_pTarget);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tSpawnInfo.vPos), 1.f));
	}
	m_eState = Idle;
	return S_OK;
}

_bool CHomonculus::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return false;

	Set_State(m_eState, fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return false;
}

void CHomonculus::LateTick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif
}

HRESULT CHomonculus::Render()
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
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return E_FAIL;
	}

	return S_OK;
}

void CHomonculus::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	if (pOther->CompareTag(L"Player_Sword"))
	{
		((CSword*)pOther)->GetDamage();
	}
	int a = 10;
}

void CHomonculus::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CHomonculus::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CHomonculus::SetNaviIndex(_uint iIndex)
{
	//m_pNavigationCom->in
}

void CHomonculus::Set_State(ANIM_STATE eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CHomonculus::Attack_02_fin:
		break;
	case CHomonculus::Attack_02_frappe:
		break;
	case CHomonculus::Attack_03:
		break;
	case CHomonculus::Big_Combo:
		break;
	case CHomonculus::Death_01:
		if (m_bAnimEnd)
		{
			m_bActive = false;
		}
		break;
	case CHomonculus::Death_02:
		break;
	case CHomonculus::Explode:
		if (m_bAnimEnd)
		{
			m_eState = Death_01;
			m_pModelCom->Change_Animation(Death_01, 0.25f, false);
		}
		break;
	case CHomonculus::GetUp:
		break;
	case CHomonculus::Hit01:
		break;
	case CHomonculus::Idle:
		if (nullptr != m_pTarget)
		{
			m_eState = Walk;
			m_pModelCom->Change_Animation(Walk);
		}
		break;
	case CHomonculus::PostCritic:
		break;
	case CHomonculus::Strafe_Left:
		break;
	case CHomonculus::Walk:
		WalkState(fTimeDelta);
		break;
	case CHomonculus::Stun:
		break;
	default:
		break;
	}
}

void CHomonculus::GetDamaged(_float fDamage)
{
	m_fHp -= fDamage;
	//체력이 0 이하라면.
	if (m_fHp <= 0.0f)
	{
		m_eState = Death_01;
		m_pModelCom->Change_Animation(Death_01, 0.25f, false);
	}
}

void CHomonculus::WalkState(_float fTimeDelta)
{
	_vector vLook = XMVectorSetY(XMVector3Normalize(m_pTarget->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.0f);
	m_pTransformCom->LookDir(vLook);
	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	if (4.5f > XMVectorGetX(XMVector3Length(m_pTarget->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
	{
		m_eState = Explode;
		m_pModelCom->Change_Animation(Explode, 0.25f, false);
	}
}

HRESULT CHomonculus::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.5f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

CHomonculus * CHomonculus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHomonculus*		pInstance = new CHomonculus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHomonculus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHomonculus::Clone(void * pArg)
{
	CHomonculus*		pInstance = new CHomonculus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHomonculus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHomonculus::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
