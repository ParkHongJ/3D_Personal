#include "stdafx.h"
#include "..\Public\Ras_Hands3.h"
#include "GameInstance.h"
#include "Ras_Samrah.h"

CRas_Hands3::CRas_Hands3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRas_Hands3::CRas_Hands3(const CRas_Hands3 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRas_Hands3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRas_Hands3::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "Ras_Samrah_Hands3");
	m_Tag = L"Ras_Samrah_Hands3";
	m_pModelCom->Set_AnimIndex(0);
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 1.f));
	return S_OK;
}

_bool CRas_Hands3::Tick(_float fTimeDelta)
{

	//Set_State(m_eState, fTimeDelta);
	return false;
}

void CRas_Hands3::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRas_Hands3::Render()
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

void CRas_Hands3::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CRas_Hands3::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Hands3::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CRas_Hands3::GetDamaged(_float fDamage)
{
	((CRas_Samrah*)m_pRasTransform->GetOwner())->GetDamaged(fDamage);
}

void CRas_Hands3::Set_State(STATE_ANIM eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CRas_Hands3::HAND_AOE1:
		if (m_bAnimEnd)
		{
			m_eState = HAND_AOE2;
			m_pModelCom->Change_Animation(HAND_AOE2, 0.0f, false);
		}
		break;
	case CRas_Hands3::HAND_AOE2:
		if (m_bAnimEnd)
		{
			m_eState = HAND_AOE3;
			m_pModelCom->Change_Animation(HAND_AOE3, 0.0f, false);
		}
		//바로 AOE1로 가는 경우도 있음.
		break;
	case CRas_Hands3::HAND_AOE3:
		break;
	case CRas_Hands3::HAND_FIRST_CLOSED:
		//추적이 끝났다면
		m_fCurrentChaseTime += fTimeDelta;
		if (m_fCurrentChaseTime > m_fChaseTimeMax)
		{
			m_eState = HAND_AOE1;
			m_pModelCom->Change_Animation(HAND_AOE1, 0.0f, false);
			m_fCurrentChaseTime = 0.0f;
			m_bChase = false;
		}
		else
		{
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			vPosition = XMVectorSetY(vPosition, 0.0f);

			if (nullptr != m_pTarget)
			{
				_vector vTargetPos = m_pTarget->Get_State(CTransform::STATE_POSITION);

				vTargetPos = XMVectorSetY(vTargetPos, 0.0f);

				vPosition = XMVectorLerp(vPosition, vTargetPos, fTimeDelta * m_fSpeed);

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

				//Look 조절해야함
				_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pRasTransform->Get_State(CTransform::STATE_POSITION);

				vLook = XMVectorSetY(vLook, 0.0f);

				m_pTransformCom->LookDir(vLook);

			}
		}
		break;
	case CRas_Hands3::HAND_SLAM_FLY:
		if (m_bAnimEnd)
		{
			m_eState = HAND_FIRST_CLOSED;
			m_pModelCom->Change_Animation(HAND_FIRST_CLOSED);
			m_bChase = true;
		}
		break;
	case CRas_Hands3::HAND_DEATH:
		//다 죽었다면.
		if (m_bAnimEnd)
		{
			m_bActive = false;
		}
		break;
	case CRas_Hands3::HAND_IDLE:
		break;
	default:
		break;
	}
}

void CRas_Hands3::SetRas_Samrah(CTransform * pRasTransform)
{
	m_pRasTransform = pRasTransform;
	Safe_AddRef(m_pRasTransform);
}

void CRas_Hands3::Set_Target(CTransform* pTarget)
{
	//타겟이 없거나 이미 있다면.
	if (nullptr == pTarget || nullptr != m_pTarget)
		return;
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CRas_Hands3::Set_Pattern(STATE_ANIM eState)
{
	m_eState = eState;
	m_pModelCom->Change_Animation(eState, 0.25f, false);
}

HRESULT CRas_Hands3::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CRas_Hands3 * CRas_Hands3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRas_Hands3*		pInstance = new CRas_Hands3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRas_Hands3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRas_Hands3::Clone(void * pArg)
{
	CRas_Hands3*		pInstance = new CRas_Hands3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRas_Hands3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRas_Hands3::Free()
{
	__super::Free();

	Safe_Release(m_pTarget);
	Safe_Release(m_pRasTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
