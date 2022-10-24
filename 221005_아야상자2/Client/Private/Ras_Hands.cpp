#include "stdafx.h"
#include "..\Public\Ras_Hands.h"
#include "GameInstance.h"
#include "Ras_Samrah.h"
#include "Sword.h"
CRas_Hands::CRas_Hands(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRas_Hands::CRas_Hands(const CRas_Hands & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRas_Hands::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRas_Hands::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	strcpy_s(m_szName, "Ras_Samrah_Hands");
	m_Tag = L"Ras_Samrah_Hands";
	m_pModelCom->Set_AnimIndex(HAND_IDLE);
	m_pTransformCom->Set_Scale(XMVectorSet(0.4f, 0.4f, 0.4f, 1.f));
	m_bActive = false;
	return S_OK;
}

_bool CRas_Hands::Tick(_float fTimeDelta)
{
	if (m_bDestroy)
		return true;
	if (!m_bActive)
		return false;

	Set_State(m_eState, fTimeDelta);

	if (m_bHitDelay)
	{
		m_fCurrentDelayTime += fTimeDelta;
		if (m_fCurrentDelayTime > m_fMaxDelayTime)
		{
			m_bHitDelay = false;
			m_fCurrentDelayTime = 0.f;
		}
	}
	for (auto& pCollider : m_pColliderCom)
	{
		if (nullptr != pCollider)
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
	}
	return false;
}

void CRas_Hands::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom || !m_bActive)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	if (m_bHitEnabled)
	{
		m_pColliderCom[COLLIDERTYPE_SPHERE]->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom[COLLIDERTYPE_SPHERE]);
	}
	if (m_bAttackEnabled)
	{
		m_pColliderCom[COLLIDERTYPE_OBB]->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom[COLLIDERTYPE_OBB]);
		m_bAttackEnabled = false;
		m_bHitEnabled = true;
	}
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CRas_Hands::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_Cut", &m_fCut, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
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
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iTemp)))
			return E_FAIL;
	}

#ifdef _DEBUG
	/*for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}*/
	if (m_bHitEnabled)
	{
		m_pColliderCom[COLLIDERTYPE_SPHERE]->Render();
	}
	if (m_bAttackEnabled)
	{
		m_pColliderCom[COLLIDERTYPE_OBB]->Render();
	}
#endif

	return S_OK;
}

void CRas_Hands::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	
}

void CRas_Hands::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	if (pOther->CompareTag(L"Player_Sword") && m_bHitEnabled)
	{
		if (!m_bHitDelay)
		{
			m_bHitDelay = true;
			((CRas_Samrah*)m_pRasTransform->GetOwner())->GetDamaged(((CSword*)pOther)->GetDamage());
		}
	}
}

void CRas_Hands::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	m_bHitEnabled = true;
	int a = 10;
}

void CRas_Hands::GetDamaged(_float fDamage)
{
	 ((CRas_Samrah*)m_pRasTransform->GetOwner())->GetDamaged(fDamage);
}

void CRas_Hands::Set_State(STATE_ANIM eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CRas_Hands::HAND_AOE1:
		if (m_bAnimEnd)
		{
			m_eState = HAND_AOE2;
			m_pModelCom->Change_Animation(HAND_AOE2, 0.0f, false);
		}
		break;
	case CRas_Hands::HAND_AOE2:
		if (m_bAnimEnd)
		{
			m_eState = HAND_AOE3;
			m_pModelCom->SetSpeed(HAND_AOE3, 80.f);
			m_pModelCom->Change_Animation(HAND_AOE3, 0.0f, false);
		}
		//바로 AOE1로 가는 경우도 있음.
		break;
	case CRas_Hands::HAND_AOE3:
		if (m_bAnimEnd)
		{
			if (!m_bAttackEnabled && !m_bHitEnabled)
			{
				m_bAttackEnabled = true;
			}
			m_fAttackTime += fTimeDelta;

			//땅바닥에 내리치고 피격이 가능한시간.
			//이제 피격이 안되면
			if (m_fAttackTime > m_fAttackTimeMax)
			{
				m_bHitEnabled = false;
				m_fAttackTime = 0.f;
				m_bAttackEnabled = false;

				//이때 사라져야함
				m_bActive = false;
				m_eState = HAND_IDLE;
				m_pModelCom->Change_Animation(HAND_IDLE);
			}
		}
		break;
	case CRas_Hands::HAND_FIRST_CLOSED:
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
			//추적중이라면 플레이어를 바라본 상태로 추적
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


			if (nullptr != m_pTarget)
			{
				_vector vTargetPos = m_pTarget->Get_State(CTransform::STATE_POSITION);

				//Look 조절해야함
				_vector vLook = vPosition - m_pRasTransform->Get_State(CTransform::STATE_POSITION);

				//타겟을 바라보는 룩을얻어와서
				_vector vDist = XMVector3Normalize(vTargetPos - m_pRasTransform->Get_State(CTransform::STATE_POSITION));
				vDist = XMVectorSetY(vDist, 0.f);
				vTargetPos = vTargetPos - vDist * 8.f;


				vPosition = XMVectorLerp(vPosition, vTargetPos, fTimeDelta * m_fSpeed);

				//네비를 태울까?
				vPosition = XMVectorSetY(vPosition, XMVectorGetY(vTargetPos) + 6.f);

				vLook = XMVectorSetY(vLook, 0.0f);

				m_pTransformCom->LookDir(vLook);

				XMVectorSetY(vLook, 0.0f);

				//vPosition -= XMVector3Normalize(vTargetPos - vPosition) * 5.f;

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

			}
		}
		break;
	case CRas_Hands::HAND_SLAM_FLY:
	{
		//추적중이라면 플레이어를 바라본 상태로 추적
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		if (nullptr != m_pTarget)
		{
			_vector vTargetPos = m_pTarget->Get_State(CTransform::STATE_POSITION);

			//Look 조절해야함
			_vector vLook = vPosition - m_pRasTransform->Get_State(CTransform::STATE_POSITION);

			//타겟을 바라보는 룩을얻어와서
			_vector vDist = XMVector3Normalize(vTargetPos - m_pRasTransform->Get_State(CTransform::STATE_POSITION));
			vDist = XMVectorSetY(vDist, 0.f);
			vTargetPos = vTargetPos - vDist * 8.f;


			vPosition = XMVectorLerp(vPosition, vTargetPos, fTimeDelta * m_fSpeed);

			//네비를 태울까?
			vPosition = XMVectorSetY(vPosition, XMVectorGetY(vTargetPos) + 6.f);

			vLook = XMVectorSetY(vLook, 0.0f);

			m_pTransformCom->LookDir(vLook);

			XMVectorSetY(vLook, 0.0f);

			//vPosition -= XMVector3Normalize(vTargetPos - vPosition) * 5.f;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		}
		if (m_bAnimEnd)
		{
			m_eState = HAND_FIRST_CLOSED;
			m_pModelCom->Change_Animation(HAND_FIRST_CLOSED);
			m_bChase = true;
		}
	}
	break;
	case CRas_Hands::HAND_DEATH:
		//다 죽었다면.
		if (m_bAnimEnd)
		{
			m_bActive = false;
		}
		break;
	case CRas_Hands::HAND_IDLE:
		break;
	default:
		break;
	}
}

void CRas_Hands::Set_Death()
{
	m_eState = HAND_DEATH;
	m_bAnimEnd = false;
	m_pModelCom->Change_Animation(HAND_DEATH, 0.25f, false);
}

void CRas_Hands::SetRas_Samrah(CTransform * pRasTransform)
{
	m_pRasTransform = pRasTransform;
	Safe_AddRef(m_pRasTransform);
}

void CRas_Hands::Set_Target(CTransform* pTarget)
{
	//타겟이 없거나 이미 있다면.
	if (nullptr == pTarget || nullptr != m_pTarget)
		return;
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
}

void CRas_Hands::Set_Pattern(STATE_ANIM eState)
{
	m_bActive = true;
	m_eState = eState;
	m_pModelCom->Change_Animation(eState, 0.25f, false);
}

HRESULT CRas_Hands::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hand1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	/* For.Com_OBB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(8.f, 3.f, 10.f);
	ColliderDesc.vCenter = _float3(0.f, -13.f, 25.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_OBB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(8.f,8.f, 8.f);
	ColliderDesc.vCenter = _float3(0.f, -10.f, 25.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CRas_Hands * CRas_Hands::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRas_Hands*		pInstance = new CRas_Hands(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRas_Hands"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRas_Hands::Clone(void * pArg)
{
	CRas_Hands*		pInstance = new CRas_Hands(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRas_Hands"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRas_Hands::Free()
{
	__super::Free();

	for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);
	Safe_Release(m_pRasTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
