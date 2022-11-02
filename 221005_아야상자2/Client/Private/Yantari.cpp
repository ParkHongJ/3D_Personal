#include "stdafx.h"
#include "..\Public\Yantari.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sword.h"

CYantari::CYantari(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CYantari::CYantari(const CYantari & rhs)
	: CGameObject(rhs)
{
}

HRESULT CYantari::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYantari::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	strcpy_s(m_szName, "Yantari");
	m_Tag = L"Yantari";

	m_eAnimState = IDLE;
	m_pModelCom->Change_Animation(IDLE);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->GetCellPos(1));
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Player", L"Com_Transform", 0);
	Safe_AddRef(m_pTargetTransform);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool CYantari::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return false;
	if (m_bDestroy)
		return true;
	Set_State(m_eAnimState, fTimeDelta);
	
	//�����⸦ ��� �� �� ���ٸ�. ��Ÿ������.
	if (!m_bCanDashAttack)
	{
		m_fCurrentDashAttackTime += fTimeDelta;
		if (m_fCurrentDashAttackTime >= m_fMaxDashAttackTime)
		{
			m_fCurrentDashAttackTime = 0.0f;
			m_bCanDashAttack = true;
		}
	}
	if (m_bHitDelay)
	{
		m_fCurrentHitDelayTime += fTimeDelta;
		if (m_fCurrentHitDelayTime > m_fMaxHitDelayTime)
		{
			m_bHitDelay = false;
			m_fCurrentHitDelayTime = 0.f;
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	Update_Weapon();

	for (auto& pPart : m_Parts)
	{
		pPart->Tick(fTimeDelta);
	}

	return false;
}

void CYantari::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	if (!m_bActive)
		return;
	if (m_bDestroy)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	//���߿� �̰� �����ض�
	for (auto& pPart : m_Parts)
	{
		pPart->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	}


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f);
	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pColliderCom->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom);
	
#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // DEBUG

}

HRESULT CYantari::Render()
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

void CYantari::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
}

void CYantari::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	if (pOther->CompareTag(L"Player_Sword") && m_bHitEnabled)
	{
		if (!m_bHitDelay)
		{
			m_bHitDelay = true;
			GetDamage(((CSword*)pOther)->GetDamage());
		}
	}
}

void CYantari::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CYantari::Ready_Layer_GameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, LEVEL_GAMEPLAY, pLayerTag, pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CYantari::GetDamage(_float fDamage)
{
	m_fHp -= fDamage;
	if (m_eAnimState == IDLE || m_eAnimState == WALK_BACK || m_eAnimState == WALK_FRONT || m_eAnimState == WALK_LEFT || m_eAnimState == WALK_RIGHT)
	{
		m_eAnimState = HIT_FIN;
		m_pModelCom->Change_Animation(HIT_FIN);
	}
	if (m_fHp <= 0.f)
	{
		m_eAnimState = DEATH;
		m_pModelCom->Change_Animation(DEATH);
	}
}

void CYantari::Set_State(ANIM_STATE eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CYantari::ATTACK1_2:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_3;
			m_pModelCom->Change_Animation(ATTACK1_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_3:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_4;
			m_pModelCom->Change_Animation(ATTACK1_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_4:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK5_BIS;
			m_pModelCom->Change_Animation(ATTACK5_BIS, 0.f, false);
		}
		else
		{
			//�÷��̾�� �� ������ �Ÿ��� �����̻��̶��. �뽬
			_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vDistance = XMVector3Length(vTargetPos - vPos);
			if (XMVectorGetX(vDistance) > 5.f)
			{
				m_pTransformCom->Go_Straight(fTimeDelta * m_fDashSpeed);
			}
		}
		break;

	case CYantari::ATTACK5_BIS:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK5_BIS2;
			m_pModelCom->Change_Animation(ATTACK5_BIS2, 0.f, false);
		}
		break;
	case CYantari::ATTACK5_BIS2:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_6;
			m_pModelCom->Change_Animation(ATTACK1_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_6:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_7;
			m_pModelCom->Change_Animation(ATTACK1_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_7:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
			m_bCanDashAttack = false;
		}
		break;
	case CYantari::ATTACK1_8:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK1_9;
			m_pModelCom->Change_Animation(ATTACK1_9, 0.f, false);
		}
		break;
	case CYantari::ATTACK1_9:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK2_2:
		m_pModelCom->SetSpeed(ATTACK2_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_3;
			m_pModelCom->Change_Animation(ATTACK2_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_3:
		m_pModelCom->SetSpeed(ATTACK2_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_4;
			m_pModelCom->Change_Animation(ATTACK2_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_4:
		m_pModelCom->SetSpeed(ATTACK2_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_5;
			m_pModelCom->Change_Animation(ATTACK2_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_5:
		m_pModelCom->SetSpeed(ATTACK2_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_6;
			m_pModelCom->Change_Animation(ATTACK2_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK2_6:
		m_pModelCom->SetSpeed(ATTACK2_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE, 0.f, false);
		}
		break;
		//ATTACK2 END, START ��Ʈ�� ���޺���
	case CYantari::ATTACK2_END:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK2_START:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK2_END;
			m_pModelCom->Change_Animation(ATTACK2_END, 0.f, false);
		}
		break;
		//ATTACK3 ��ü�� ���޺�
	case CYantari::ATTACK3:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK3_2:
		m_pModelCom->SetSpeed(ATTACK3_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_3;
			m_pModelCom->Change_Animation(ATTACK3_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_3:
		m_pModelCom->SetSpeed(ATTACK3_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_4;
			m_pModelCom->Change_Animation(ATTACK3_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_4:
		m_pModelCom->SetSpeed(ATTACK3_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_5;
			m_pModelCom->Change_Animation(ATTACK3_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_5:
		m_pModelCom->SetSpeed(ATTACK3_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_6;
			m_pModelCom->Change_Animation(ATTACK3_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_6:
		m_pModelCom->SetSpeed(ATTACK3_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_7;
			m_pModelCom->Change_Animation(ATTACK3_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_7:
		m_pModelCom->SetSpeed(ATTACK3_7, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK3_8;
			m_pModelCom->Change_Animation(ATTACK3_8, 0.f, false);
		}
		break;
	case CYantari::ATTACK3_8:
		m_pModelCom->SetSpeed(ATTACK3_8, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::ATTACK4_2:
		m_pModelCom->SetSpeed(ATTACK4_2, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_3;
			m_pModelCom->Change_Animation(ATTACK4_3, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_3:
		m_pModelCom->SetSpeed(ATTACK4_3, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_4;
			m_pModelCom->Change_Animation(ATTACK4_4, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_4:
		m_pModelCom->SetSpeed(ATTACK4_4, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_5;
			m_pModelCom->Change_Animation(ATTACK4_5, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_5:
		m_pModelCom->SetSpeed(ATTACK4_5, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_6;
			m_pModelCom->Change_Animation(ATTACK4_6, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_6:
		m_pModelCom->SetSpeed(ATTACK4_6, m_fSpeed);
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::ATTACK4_7;
			m_pModelCom->Change_Animation(ATTACK4_7, 0.f, false);
		}
		break;
	case CYantari::ATTACK4_7:
		if (m_bAnimEnd)
		{
			m_eAnimState = CYantari::IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
		//���߰� �ֺ� ���û���������
	case CYantari::CAST:
		break;
	case CYantari::DEATH:
		if (m_bAnimEnd)
		{
			m_bActive = false;
		}
		break;
	case CYantari::GETUP:
		break;
	case CYantari::HIT:
		break;
	case CYantari::HIT_DEBUT:
		break;
	case CYantari::HIT_FIN:
		if (m_bAnimEnd)
		{
			m_eAnimState = IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
		break;
	case CYantari::IDLE:
	{
#pragma region �÷��̾� �ٶ󺸱�
		//Look �����ؾ���
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDistance = XMVector3Length(vTargetPos - vMyPos);

		//Y���� ���ּ� ���Ʒ� ������� �÷��̾ �ٶ󺸰���
		_vector vLook = XMVector3Normalize(vTargetPos - vMyPos);
		vLook = XMVectorSetY(vLook, 0.0f);

		_vector vMyLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.0f);

		//ȸ���� ����������
		if (!XMVector3Equal(vLook, vMyLook))
		{
			m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
		}
#pragma endregion


		//�ִ� �޺����� �Ѿ��ٸ�.
		if (m_iCurrentCombo >= m_iMaxCombo)
		{
			if (XMVectorGetX(vDistance) < 5.f)
			{
				_uint iRand = rand() % 3 + 1;
				switch (iRand)
				{
				case 1:
					m_eAnimState = WALK_LEFT;
					m_pModelCom->Change_Animation(WALK_LEFT);
					break;
				case 2:
					m_eAnimState = WALK_RIGHT;
					m_pModelCom->Change_Animation(WALK_RIGHT);
					break;
				case 3:
					break;
				default:
					break;
				}
			}
			m_fCurrentDelayTime += fTimeDelta;
			//�ִ� �޺����� �Ѱ� ���½ð��� �����ٸ�
			if (m_fCurrentDelayTime >= m_fMaxDelayTime)
			{
				m_iCurrentCombo = 0;
				m_fCurrentDelayTime = 0.0f;
			}
		}
		//�ִ� �޺����� ���� �ʾҴٸ�.(�޺����� ���)
		else
		{
			m_fCurrentGlobalDelayTime += fTimeDelta;
			if (m_fCurrentGlobalDelayTime >= m_fGlobalMaxDelayTime)
			{
				m_fCurrentGlobalDelayTime = 0.0f;
				//�÷��̾�� ���� �Ÿ��� 4.5 �̻��̶��. ����
				if (XMVectorGetX(vDistance) > 4.5f)
				{
					//�뽬������ �����ϴٸ� �뽬����
					if (m_bCanDashAttack)
					{
						m_pTransformCom->LookDir(vLook);
						m_eAnimState = CYantari::ATTACK1_2;
						m_pModelCom->Change_Animation(ATTACK1_2, 0.0f, false);
						m_iCurrentCombo++;
					}
					else
					{
						//�뽬������ �������� �ʰ� ���� �Ÿ��� �ִٸ� �����ΰ�.
						m_eAnimState = WALK_FRONT;
						m_pModelCom->Change_Animation(WALK_FRONT);
					}
				}
				else
				{
					//���� �Ÿ��� ������.(�뽬������ �Ұ����ϰ�) 
					_uint iRand = rand() % 4 + 1;
					switch (iRand)
					{
					case 1:
						m_iCurrentCombo--;
						break;
					case 2:
						m_eAnimState = CYantari::ATTACK2_2;
						m_pModelCom->Change_Animation(ATTACK2_2, 0.0f, false);
						break;
					case 3:
						m_eAnimState = CYantari::ATTACK3_2;
						m_pModelCom->Change_Animation(ATTACK3_2, 0.0f, false);
						break;
					case 4:
						m_eAnimState = CYantari::ATTACK4_2;
						m_pModelCom->Change_Animation(ATTACK4_2, 0.0f, false);						
						break;
					}
					m_iCurrentCombo++;
				}
			}
		}
	}
		break;
	case CYantari::POST_CRITIC:
		break;
	case CYantari::WALK_BACK:
	{
#pragma region �÷��̾� �ٶ󺸱�
		//Look �����ؾ���
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDistance = XMVector3Length(vTargetPos - vMyPos);

		//Y���� ���ּ� ���Ʒ� ������� �÷��̾ �ٶ󺸰���
		_vector vLook = XMVector3Normalize(vTargetPos - vMyPos);
		vLook = XMVectorSetY(vLook, 0.0f);

		_vector vMyLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.0f);

		//ȸ���� ����������
		if (!XMVector3Equal(vLook, vMyLook))
		{
			m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Backward(fTimeDelta * m_fMoveSpeed);
#pragma endregion
	}
		break;
	case CYantari::WALK_FRONT:
	{
#pragma region �÷��̾� �ٶ󺸱�
		//Look �����ؾ���
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDistance = XMVector3Length(vTargetPos - vMyPos);

		//Y���� ���ּ� ���Ʒ� ������� �÷��̾ �ٶ󺸰���
		_vector vLook = XMVector3Normalize(vTargetPos - vMyPos);
		vLook = XMVectorSetY(vLook, 0.0f);

		_vector vMyLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.0f);

		//ȸ���� ����������
		if (!XMVector3Equal(vLook, vMyLook))
		{
			m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
		}
#pragma endregion

		//�����Ÿ� �̻��̸� ����
		if (XMVectorGetX(vDistance) > 3.f)
		{
			m_pTransformCom->Go_Straight(fTimeDelta* m_fMoveSpeed);
			if (m_bCanDashAttack)
			{
				m_pTransformCom->LookDir(vLook);
				m_eAnimState = CYantari::ATTACK1_2;
				m_pModelCom->Change_Animation(ATTACK1_2, 0.0f, false);
			}
		}
		else
		{
			//�����Ÿ� ���϶��.
			m_eAnimState = IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
	}
		break;
	case CYantari::WALK_LEFT:
	{
#pragma region �÷��̾� �ٶ󺸱�
		//Look �����ؾ���
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDistance = XMVector3Length(vTargetPos - vMyPos);

		//Y���� ���ּ� ���Ʒ� ������� �÷��̾ �ٶ󺸰���
		_vector vLook = XMVector3Normalize(vTargetPos - vMyPos);
		vLook = XMVectorSetY(vLook, 0.0f);

		_vector vMyLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.0f);

		//ȸ���� ����������
		if (!XMVector3Equal(vLook, vMyLook))
		{
			m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Left(fTimeDelta* m_fMoveSpeed);
#pragma endregion

		m_fCurrentDelayTime += fTimeDelta;
		//�ִ� �޺����� �Ѱ� ���½ð��� �����ٸ�
		if (m_fCurrentDelayTime >= m_fMaxDelayTime)
		{
			m_iCurrentCombo = 0;
			m_eAnimState = IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
	}
		break;
	case CYantari::WALK_RIGHT:
	{
#pragma region �÷��̾� �ٶ󺸱�
		//Look �����ؾ���
		_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vDistance = XMVector3Length(vTargetPos - vMyPos);

		//Y���� ���ּ� ���Ʒ� ������� �÷��̾ �ٶ󺸰���
		_vector vLook = XMVector3Normalize(vTargetPos - vMyPos);
		vLook = XMVectorSetY(vLook, 0.0f);

		_vector vMyLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.0f);

		//ȸ���� ����������
		if (!XMVector3Equal(vLook, vMyLook))
		{
			m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Right(fTimeDelta* m_fMoveSpeed);
#pragma endregion
		m_fCurrentDelayTime += fTimeDelta;
		//�ִ� �޺����� �Ѱ� ���½ð��� �����ٸ�
		if (m_fCurrentDelayTime >= m_fMaxDelayTime)
		{
			m_iCurrentCombo = 0;
			m_eAnimState = IDLE;
			m_pModelCom->Change_Animation(IDLE);
		}
	}
		break;
	default:
		break;
	}
}


HRESULT CYantari::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("Root_Axe_R");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CYantari::Ready_Parts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.YantariWeapon */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_YantariWeapon"));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_Parts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CYantari::Update_Weapon()
{
	/*if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;*/

	/* ���. */
	/*_matrix			WeaponMatrix = ���� �����̽� ��ȯ(OffsetMatrix)
	* ���� ���(CombinedTransformation)
	* ���� PivotMatrix * �����̾��ǿ������. ;*/

	_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		* m_pTransformCom->Get_WorldMatrix();

	m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);
	return S_OK;
}

HRESULT CYantari::Ready_Components()
{
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = 1.f;

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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Yantari"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.5f, 4.f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

CYantari * CYantari::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYantari*		pInstance = new CYantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYantari"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CYantari::Clone(void * pArg)
{
	CYantari*		pInstance = new CYantari(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYantari"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYantari::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
	{
		Safe_Release(pPart);
	}
	m_Parts.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
