#include "stdafx.h"
#ifdef _DEBUG
#include "..\Default\Imgui\imgui.h"
#include "..\Default\Imgui\imgui_impl_dx11.h"
#include "..\Default\Imgui\imgui_impl_win32.h"
#endif // _DEBUG
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Camera_Free.h"
#include "GameMgr.h"
#include "UI_Manager.h"
#include "Sword.h"
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(IdlePeace);
	m_eCurrentAnimState = CPlayer::IdlePeace;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	strcpy_s(m_szName, "Player");

	CGameMgr* pGameMgr = GET_INSTANCE(CGameMgr);
	pGameMgr->RegisterPlayer(this);
	RELEASE_INSTANCE(CGameMgr);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->GetCellPos(0));
	return S_OK;
}

_bool CPlayer::Tick(_float fTimeDelta)
{
	if (m_bDestroy)
		return true;

	SetState(m_eCurrentAnimState, fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_Y))
	{
		pGameInstance->Add_GameObjectToLayer(L"Prototype_GameObject_Hit_Effect", LEVEL_GAMEPLAY, L"Effect");
	}
	
	RELEASE_INSTANCE(CGameInstance);
	if (m_bIncreaseStamina)
	{
		IncreaseStamina(fTimeDelta, 70.f);
	}
	Update_Weapon();

#ifdef _DEBUG
	static _float fWhite = 1.5f;
	static _float fMiddleGrey = 1.5f;

	m_pRendererCom->GetParameters(fMiddleGrey, fWhite);
	ImGui::Begin("PostProcess");
	//ImGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
	ImGui::DragFloat("fMiddleGrey", &fMiddleGrey, 0.001f, 0.1f, 6.0f);
	ImGui::DragFloat("fWhite", &fWhite, 0.001f, 0.1f, 6.0f);
	ImGui::End();
	m_pRendererCom->SetParameters(fMiddleGrey, fWhite);
#endif // _DEBUG


	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);
	m_pColliderCom[COLLIDERTYPE_OBB]->Update(m_pTransformCom->Get_WorldMatrix());

	return false;
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_bAnimEnd = m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pPart : m_Parts)
	{
		if (m_bWeaponEnable)
		{
			pPart->LateTick(fTimeDelta);
		}
	}
	for (auto& pPart : m_Parts)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	}
	m_pColliderCom[COLLIDERTYPE_OBB]->Add_CollisionGroup(CCollider_Manager::MONSTER, m_pColliderCom[COLLIDERTYPE_OBB]);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugGroup(m_pColliderCom[COLLIDERTYPE_OBB]);
	m_pRendererCom->Add_DebugGroup(m_pNavigationCom);
#endif // DEBUG

}

HRESULT CPlayer::Render()
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

void CPlayer::SetState(STATE_ANIM eState, _float fTimeDelta)
{
	switch (eState)
	{
	case CPlayer::Backstab_Deb_1:
		break;
	case CPlayer::Backstab_Deb_2:
		break;
	case CPlayer::Backstab_Deb_3:
		break;
	case CPlayer::Backstab_Fin:
		break;
	case CPlayer::Backstab_Fin_inter:
		break;
	case CPlayer::DashAir:
		break;
	case CPlayer::DashBack:
		m_fDashCurrentTime += fTimeDelta;
		if (m_fDashCurrentTime >= m_fDashMaxTime)
		{
			m_fDashCurrentTime = 0.0f;
			m_eCurrentAnimState = IdleFight;
			m_pModelCom->Change_Animation(IdleFight);
		}
		else
		{
			_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			_vector vMyLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			if (!XMVector3Equal(vLook, vMyLook))
			{
				m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
			}
			m_pTransformCom->Go_Backward(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
		}
		break;
	case CPlayer::DashFront:
		m_fDashCurrentTime += fTimeDelta;
		if (m_fDashCurrentTime >= m_fDashMaxTime)
		{
			m_fDashCurrentTime = 0.0f;
			m_eCurrentAnimState = IdleFight;
			m_pModelCom->Change_Animation(IdleFight);
		}
		else
		{
			_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			_vector vMyLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			if (!XMVector3Equal(vLook, vMyLook))
			{
				m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
			}
			m_pTransformCom->Go_Straight(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
		}
		break;
	case CPlayer::DashLeft:
		m_fDashCurrentTime += fTimeDelta;
		if (m_fDashCurrentTime >= m_fDashMaxTime)
		{
			m_fDashCurrentTime = 0.0f;
			m_eCurrentAnimState = IdleFight;
			m_pModelCom->Change_Animation(IdleFight);
		}
		else
		{
			_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			_vector vMyLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			if (!XMVector3Equal(vLook, vMyLook))
			{
				m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
			}
			m_pTransformCom->Go_Left(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
		}
		break;

	case CPlayer::dash_right_v2:
		m_fDashCurrentTime += fTimeDelta;
		if (m_fDashCurrentTime >= m_fDashMaxTime)
		{
			m_fDashCurrentTime = 0.0f;
			m_eCurrentAnimState = IdleFight;
			m_pModelCom->Change_Animation(IdleFight);
		}
		else
		{
			_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
			_vector vMyLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
			if (!XMVector3Equal(vLook, vMyLook))
			{
				m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
			}
			m_pTransformCom->Go_Right(fTimeDelta * m_fDashSpeed, m_pNavigationCom);
		}
		break;
	case CPlayer::Death:
		break;
	case CPlayer::DeathLong:
		break;
	case CPlayer::DoubleJumpCloth_Start:
		Jump(fTimeDelta);
		break;
	case CPlayer::Health:
		break;
	case CPlayer::IdleFight:
		Idle_Fight_State(fTimeDelta);
		break;
	case CPlayer::IdlePeace:
		Idle_State(fTimeDelta);
		break;
	case CPlayer::JumpCloth_Air:
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Key_Down(DIK_SPACE))
		{
			m_bJumping = true;
			m_fJumpTime = 0.0f;
			m_fPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pModelCom->Change_Animation(DoubleJumpCloth_Start, 0.25f, false);
			m_eCurrentAnimState = DoubleJumpCloth_Start;
		}
		RELEASE_INSTANCE(CGameInstance);
		MoveControl(fTimeDelta);
		Jump(fTimeDelta);
	}
	break;
	case CPlayer::JumpCloth_Land:
	{
		if (m_bAnimEnd)
		{
			m_eCurrentAnimState = IdlePeace;
			m_pModelCom->Change_Animation(IdlePeace);
		}
		MoveControl(fTimeDelta);
	}
	break;
	case CPlayer::JumpCloth_Start:
		//if (m_bAnimEnd)
		//{
		//	m_eCurrentAnimState = JumpCloth_Air;
		//	m_pModelCom->Change_Animation(JumpCloth_Air, 0.15f, false);
		//	//Jump(fTimeDelta);
		//	break;
		//}
		//Jump(fTimeDelta);
		break;
	case CPlayer::LowFight1:
		break;
	case CPlayer::Magic:
		break;
	case CPlayer::Magic_001:
		break;
	case CPlayer::Magic_2:
		break;
	case CPlayer::Magic_3:
		break;
	case CPlayer::Magic_4:
		break;
	case CPlayer::Magic_5:
		break;
	case CPlayer::Parry:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(Parry_2, 0.f, false);
			m_eCurrentAnimState = Parry_2;
		}
		m_bCanParry = true;
		((CSword*)(m_Parts[PART_WEAPON]))->SetState(CSword::PARRY);
		break;
	case CPlayer::Parry_2:
		m_bWeaponEnable = true;
		if (m_bParry)
		{
			m_bWeaponEnable = false;
			m_pModelCom->Change_Animation(IdleFight);
			m_eCurrentAnimState = IdleFight;
			m_bCanParry = false;
		}
		else
		{
			//패링 실패시
			if (m_bAnimEnd)
			{
				m_pModelCom->Change_Animation(Parry_3, 0.0f, false);
				m_eCurrentAnimState = Parry_3;
				m_bCanParry = false;
			}
		}
		break;
	case CPlayer::Parry_3:
		m_bWeaponEnable = false;
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(IdleFight);
			m_eCurrentAnimState = IdleFight;
			((CSword*)m_Parts[PART_WEAPON])->SetState(CSword::ATTACK);
		}
		break;
	case CPlayer::Projection:
		break;
	case CPlayer::ProjectionInAir:
		break;
	case CPlayer::ProjectionLand:
		break;
	case CPlayer::RolL2:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(IdlePeace, 0.25f, true);
			m_eCurrentAnimState = IdlePeace;
			m_fRollTime = 0.0f;
		}
		else
		{
			m_fRollTime += fTimeDelta;
			if (m_fRollTime <= m_fRollTimeMax)
			{
				m_pTransformCom->Go_Straight(fTimeDelta * 1.5f, m_pNavigationCom);
			}
		}
		break;
	case CPlayer::RollAttack:
		break;
	case CPlayer::RollAttack_2:
		break;
	case CPlayer::RollAttack_3:
		break;
	case CPlayer::RollAttack_4:
		break;
	case CPlayer::RollAttack_5:
		break;
	case CPlayer::Run:
		Run_State(fTimeDelta);
		break;
	case CPlayer::Sleeping:
		break;
	case CPlayer::SleepingStandUp:
		break;
	case CPlayer::Slide:
		break;
	case CPlayer::Sprint:
		Sprint_State(fTimeDelta);
		break;
	case CPlayer::Stun:
		break;
	case CPlayer::Walk:
		break;
	case CPlayer::WalkBack:
		break;
	case CPlayer::WalkLeft:
		break;
	case CPlayer::WalkRight:
		break;
	case CPlayer::fight_coup1:
		break;
	case CPlayer::fight_coup2:
		break;
	case CPlayer::fight_coup1bis:
		break;
	case CPlayer::fight_coup3:
		break;
	case CPlayer::fight_coup4:
		break;
	case CPlayer::fight_coup5:
		break;
	case CPlayer::fight_coup6:
		break;
	case CPlayer::fight_deb:
		break;
	case CPlayer::fight_fin:
		break;
	case CPlayer::fight_inter:
		break;
	case CPlayer::fight_prep2:
		break;
	case CPlayer::dash_air_v3:
		break;
	case CPlayer::interaction_v2:
		break;
	case CPlayer::interaction_v2_fin:
		break;
	case CPlayer::power_fight_01:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(power_fight_02, 0.f, false);
			m_eCurrentAnimState = power_fight_02;
		}
		break;
	case CPlayer::power_fight_02:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(power_fight_03, 0.f, false);
			m_eCurrentAnimState = power_fight_03;
		}
		break;
	case CPlayer::power_fight_03:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(power_fight_04, 0.f, false);
			m_eCurrentAnimState = power_fight_04;
		}
		break;
	case CPlayer::power_fight_04:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(power_fight_05, 0.f, false);
			m_eCurrentAnimState = power_fight_05;
		}
		break;
	case CPlayer::power_fight_05:
		if (m_bAnimEnd)
		{
			if (m_bLockOn)
			{
				m_pModelCom->Change_Animation(IdleFight);
				m_eCurrentAnimState = IdleFight;
			}
			else
			{
				m_pModelCom->Change_Animation(IdlePeace);
				m_eCurrentAnimState = IdlePeace;
			}
		}
		break;
	case CPlayer::walkfront:
		break;
	case CPlayer::Jump6_Land:
		break;
	case CPlayer::Jump6_Start:
		break;
	case CPlayer::attaquePiquee_Land:
		break;
	case CPlayer::attaquePiquee_v3:
		break;
	case CPlayer::CoupFaible1_fin:
		if (m_bComboAttack)
		{
			m_pModelCom->Change_Animation(CoupFaible2_frappe1, 0.25f, false);
			m_eCurrentAnimState = CoupFaible2_frappe1;
			m_bComboAttack = false;
		}
		m_bWeaponEnable = false;
		if (m_bAnimEnd)
		{
			if (m_bLockOn)
			{
				m_pModelCom->Change_Animation(IdleFight);
				m_eCurrentAnimState = IdleFight;
			}
			else
			{
				m_pModelCom->Change_Animation(IdlePeace);
				m_eCurrentAnimState = IdlePeace;
			}
		}
		break;
	case CPlayer::CoupFaible1_frappe1:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(CoupFaible1_frappe2, 0.f, false);
			m_eCurrentAnimState = CoupFaible1_frappe2;
		}
		break;
	case CPlayer::CoupFaible1_frappe2:
	{
		m_bWeaponEnable = true;
		((CSword*)(m_Parts[PART_WEAPON]))->SetState(CSword::ATTACK);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Get_DIMKeyState(DIMK_LBUTTON))
		{
			m_bComboAttack = true;
		}
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(CoupFaible1_fin, 0.1f, false);
			m_eCurrentAnimState = CoupFaible1_fin;

			if (m_bComboAttack)
			{
				if (!SetStamina(m_fTestVariable))
					m_bComboAttack = false;
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}
		break;
	case CPlayer::CoupFaible1_pause:
		break;
	case CPlayer::CoupFaible1_prepa1:
		break;
	case CPlayer::CoupFaible1_prepa2:
		break;
	case CPlayer::CoupFaible2_fin:
		m_bWeaponEnable = false;
		if (m_bAnimEnd)
		{
			if (m_bLockOn)
			{
				m_pModelCom->Change_Animation(IdleFight);
				m_eCurrentAnimState = IdleFight;
			}
			else
			{
				m_pModelCom->Change_Animation(IdlePeace);
				m_eCurrentAnimState = IdlePeace;
			}
		}
		break;
	case CPlayer::CoupFaible2_frappe1:
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(CoupFaible2_frappe2, 0.f, false);
			m_eCurrentAnimState = CoupFaible2_frappe2;
		}
		break;
	case CPlayer::CoupFaible2_frappe2:
		m_bWeaponEnable = true;
		if (m_bAnimEnd)
		{
			m_pModelCom->Change_Animation(CoupFaible2_fin, 0.25f, false);
			m_eCurrentAnimState = CoupFaible2_fin;
		}
		break;
	case CPlayer::CoupFaible2_pause:
		break;
	case CPlayer::CoupFaible2_prepa1:
		break;
	case CPlayer::CoupFaible2_prepa2:
		break;
	case CPlayer::CoupFaibleCharge_v3:
		break;
	case CPlayer::CoupFaibleCharge_v3_01:
		break;
	case CPlayer::CoupFaibleCharge_v3_02:
		break;
	case CPlayer::CoupFaibleCharge_v3_03:
		break;
	case CPlayer::CoupFaibleCharge_v3_04:
		break;
	case CPlayer::CoupPuissant_v5_1:
		break;
	case CPlayer::CoupPuissant_v5_2:
		break;
	case CPlayer::CoupPuissant_v5_3:
		break;
	case CPlayer::CoupPuissant_v5_4:
		break;
	case CPlayer::CoupPuissant_v5_5:
		break;
	case CPlayer::HitFail:
		break;
	default:
		break;
	}
}

void CPlayer::Idle_State(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
#pragma region LockOn
	//락온키를 눌렀을때.
	if (pGameInstance->Key_Down(LockON))
	{
		//새로운 타겟을 등록
		Set_Target(LEVEL_YANTARI, L"Layer_Yantari", L"Com_Transform", 0);

		m_eCurrentAnimState = IdleFight;
		m_pModelCom->Change_Animation(IdleFight);
	}

#pragma endregion

	if (pGameInstance->Key_Pressing(MoveForward) || (pGameInstance->Key_Pressing(MoveBack) ||
		pGameInstance->Key_Pressing(MoveLeft) || pGameInstance->Key_Pressing(MoveRight)))
	{
		m_eCurrentAnimState = Run;

		m_pModelCom->Change_Animation(Run);
	}
	else if (pGameInstance->Key_Down(DIK_SPACE) && !m_bJumping)
	{
		m_bJumping = true;
		m_fPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->Change_Animation(JumpCloth_Air, 0.25f, false);
		m_eCurrentAnimState = JumpCloth_Air;
	}
	else if (pGameInstance->Get_DIMKeyState(DIMK_LBUTTON))
	{
		if (SetStamina(m_fTestVariable))
		{
			m_pModelCom->Change_Animation(CoupFaible1_frappe1, 0.25f, false);
			m_eCurrentAnimState = CoupFaible1_frappe1;
			m_bAnimEnd = false;
			m_fStaminaTimeCurrent = 0.f;
			m_bIncreaseStamina = false;
		}
	}
	else if (pGameInstance->Get_DIMKeyState(DIMK_RBUTTON))
	{
		m_pModelCom->Change_Animation(power_fight_01, 0.25f, false);
		m_eCurrentAnimState = power_fight_01;
		m_bAnimEnd = false;
		m_fStaminaTimeCurrent = 0.f;
		m_bIncreaseStamina = false;
	}
	else
	{
		m_fStaminaTimeCurrent += fTimeDelta;
		if (m_fStaminaTimeCurrent >= m_fStaminaTimeMax)
		{
			//스테미너 증가
			m_bIncreaseStamina = true;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Run_State(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(MoveForward) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(RolL2, 0.25f, false);
		m_eCurrentAnimState = RolL2;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveBack) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(RolL2, 0.25f, false);
		m_eCurrentAnimState = RolL2;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveLeft) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(RolL2, 0.25f, false);
		m_eCurrentAnimState = RolL2;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveRight) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(RolL2, 0.25f, false);
		m_eCurrentAnimState = RolL2;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Down(DIK_SPACE) && !m_bJumping)
	{
		m_bJumping = true;
		m_fPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->Change_Animation(JumpCloth_Air, 0.25f, false);
		m_eCurrentAnimState = JumpCloth_Air;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	if (pGameInstance->Get_DIMKeyState(DIMK_LBUTTON))
	{
		m_pModelCom->Change_Animation(CoupFaible1_frappe1, 0.25f, false);
		m_eCurrentAnimState = CoupFaible1_frappe1;
		m_bAnimEnd = false;
		m_fStaminaTimeCurrent = 0.f;
		m_bIncreaseStamina = false;
	}
	else if (pGameInstance->Get_DIMKeyState(DIMK_RBUTTON))
	{
		m_pModelCom->Change_Animation(power_fight_01, 0.25f, false);
		m_eCurrentAnimState = power_fight_01;
		m_bAnimEnd = false;
		m_fStaminaTimeCurrent = 0.f;
		m_bIncreaseStamina = false;
	}
	else if (pGameInstance->Key_Pressing(MoveForward))
	{
		if (!XMVector3Equal(XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_LOOK)), XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK))))
		{
			m_pTransformCom->TurnQuat(XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK)), fTimeDelta * m_fRotationSpeed);
		}

		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveBack))
	{
		if (!XMVector3Equal(-XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_LOOK)), -XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK))))
		{
			m_pTransformCom->TurnQuat(-XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK)), fTimeDelta * m_fRotationSpeed);
		}

		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveLeft))
	{
		if (!XMVector3Equal(-XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_RIGHT)), -XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT))))
		{
			m_pTransformCom->TurnQuat(-XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT)), fTimeDelta * m_fRotationSpeed);
		}

		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveRight))
	{
		if (!XMVector3Equal(XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_RIGHT)), XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT))))
		{
			m_pTransformCom->TurnQuat(XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT)), fTimeDelta * m_fRotationSpeed);
		}

		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else
	{
		m_pModelCom->Change_Animation((_uint)IdlePeace);
		m_eCurrentAnimState = IdlePeace;
	}

#pragma region LockOn
	//락온키를 눌렀을때.
	if (pGameInstance->Key_Down(LockON))
	{
		//새로운 타겟을 등록
		Set_Target(LEVEL_YANTARI, L"Layer_Yantari", L"Com_Transform", 0);

		m_eCurrentAnimState = IdleFight;
		m_pModelCom->Change_Animation(IdleFight);
	}

#pragma endregion


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Idle_Fight_State(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//락온키를 눌렀을때.
	if (pGameInstance->Key_Down(LockON))
	{
		//이미 타겟이 있었다면(락온 중이였다면.) 해제
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
			m_bLockOn = false;

			m_eCurrentAnimState = IdlePeace;
			m_pModelCom->Change_Animation(IdlePeace);

			m_pCamera->ReleaseTarget();

			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}
	//타겟을 향해 회전
#pragma region 타겟회전
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f));
	_vector vMyLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
	if (!XMVector3Equal(vLook, vMyLook))
	{
		m_pTransformCom->TurnQuat(vLook, fTimeDelta * m_fRotationSpeed);
	}
#pragma endregion


	if (pGameInstance->Key_Pressing(MoveForward) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(DashFront);
		m_eCurrentAnimState = DashFront;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveBack) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(DashBack);
		m_eCurrentAnimState = DashBack;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveLeft) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(DashLeft);
		m_eCurrentAnimState = DashLeft;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	else if (pGameInstance->Key_Pressing(MoveRight) && pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pModelCom->Change_Animation(dash_right_v2);
		m_eCurrentAnimState = dash_right_v2;
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (pGameInstance->Get_DIMKeyState(DIMK_LBUTTON))
	{
		m_pModelCom->Change_Animation(CoupFaible1_frappe1, 0.25f, false);
		m_eCurrentAnimState = CoupFaible1_frappe1;
		m_bAnimEnd = false;
		m_fStaminaTimeCurrent = 0.f;
		m_bIncreaseStamina = false;
	}
	else if (pGameInstance->Get_DIMKeyState(DIMK_RBUTTON))
	{
		m_pModelCom->Change_Animation(power_fight_01, 0.1f, false);
		m_eCurrentAnimState = power_fight_01;
		m_bAnimEnd = false;
		m_fStaminaTimeCurrent = 0.f;
		m_bIncreaseStamina = false;
	}
	else if (pGameInstance->Key_Pressing(MoveForward))
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		m_pModelCom->Change_Animation(walkfront, 0.1f);
	}
	else if (pGameInstance->Key_Pressing(MoveBack))
	{
		m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
		m_pModelCom->Change_Animation(WalkBack, 0.1f);
	}
	else if (pGameInstance->Key_Pressing(MoveLeft))
	{
		m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
		m_pModelCom->Change_Animation(WalkLeft, 0.1f);
	}
	else if (pGameInstance->Key_Pressing(MoveRight))
	{
		m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);
		m_pModelCom->Change_Animation(WalkRight, 0.1f);
	}
	else if (pGameInstance->Key_Down(DIK_F))
	{
		m_pModelCom->Change_Animation(Parry, 0.0f, false);
		m_eCurrentAnimState = Parry;
		((CSword*)m_Parts[PART_WEAPON])->SetState(CSword::PARRY);
	}
	else
	{
		m_pModelCom->Change_Animation(IdleFight);

		if (nullptr == m_pTargetTransform)
		{
			m_fBehaviorTimeCurrent += fTimeDelta;
			if (m_fBehaviorTimeCurrent >= m_fBehaviorTimeMax)
			{
				m_fBehaviorTimeCurrent = 0.0f;
				m_pModelCom->Change_Animation(IdlePeace);
				m_eCurrentAnimState = IdlePeace;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
		}

		m_fStaminaTimeCurrent += fTimeDelta;
		if (m_fStaminaTimeCurrent >= m_fStaminaTimeMax)
		{
			//스테미너 증가
			m_bIncreaseStamina = true;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Sprint_State(_float fTimeDelta)
{
}

void CPlayer::Parring_State(_float fTimeDelta)
{
}

void CPlayer::Jump(_float fTimeDelta)
{
	//y=-a*x+b에서 (a: 중력가속도, b: 초기 점프속도)
	//적분하여 y = (-a/2)*x*x + (b*x) 공식을 얻는다.(x: 점프시간, y: 오브젝트의 높이)
	//변화된 높이 height를 기존 높이 _posY에 더한다.
	_float height = (m_fJumpTime * m_fJumpTime * (-m_fGravity) * 0.5f) + (m_fJumpTime * m_fJumpPower);
	
	_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPos), m_fPosY + height, XMVectorGetZ(vPos), 1.f));
	//점프시간을 증가시킨다.
	m_fJumpTime += fTimeDelta;

	//처음의 높이 보다 더 내려 갔을때 => 점프전 상태로 복귀한다.
	if (height < m_pNavigationCom->GetHeight(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		m_bJumping = false;
		m_fJumpTime = 0.0f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPos), m_fPosY, XMVectorGetZ(vPos), 1.f));

		m_pModelCom->Change_Animation(JumpCloth_Land, 0.05f, false);
		m_eCurrentAnimState = JumpCloth_Land;
	}
}

void CPlayer::MoveControl(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Pressing(MoveForward))
	{
		if (!XMVector3Equal(XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_LOOK)), XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK))))
		{
			m_pTransformCom->TurnQuat(XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK)), fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveBack))
	{
		if (!XMVector3Equal(-XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_LOOK)), -XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK))))
		{
			m_pTransformCom->TurnQuat(-XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_LOOK)), fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveLeft))
	{
		if (!XMVector3Equal(-XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_RIGHT)), -XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT))))
		{
			m_pTransformCom->TurnQuat(-XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT)), fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	else if (pGameInstance->Key_Pressing(MoveRight))
	{
		if (!XMVector3Equal(XMLoadFloat3(&GetNormalizeDir(CTransform::STATE_RIGHT)), XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT))))
		{
			m_pTransformCom->TurnQuat(XMLoadFloat3(&m_pCamera->GetNormalizeDir(CTransform::STATE_RIGHT)), fTimeDelta * m_fRotationSpeed);
		}
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayer::Set_Camera(CCamera_Free * pCamera)
{
	m_pCamera = pCamera;
	if (nullptr == m_pCamera)
	{
		return E_FAIL;
	}
	Safe_AddRef(m_pCamera);
	return S_OK;
}

HRESULT CPlayer::Set_Target(_uint iLevel, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == m_pTargetTransform)
	{
		m_pTargetTransform = (CTransform*)pGameInstance->Get_ComponentPtr(iLevel, pLayerTag, pComponentTag, iLayerIndex);
		if (nullptr != m_pTargetTransform)
			Safe_AddRef(m_pTargetTransform);
	}

	m_bLockOn = true;
	m_pModelCom->Change_Animation(IdleFight);
	m_eCurrentAnimState = IdleFight;

	m_pCamera->Set_Target(m_pTargetTransform);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CPlayer::GetDamage(_float fDamage)
{
	m_fHP -= fDamage;
	if (m_fHP <= 0.f)
	{
		//죽음
		m_fHP = 0.f;
		CUI_Manager::Get_Instance()->SetValue("HealthBar", m_fHP);

		m_eCurrentAnimState = Death;
		m_pModelCom->Change_Animation(Death);
	}
	else
	{
		//UI출력
		_float fCurrentRatio = m_fHP / m_fMaxHp;
		CUI_Manager::Get_Instance()->SetValue("HealthBar", fCurrentRatio);
	}
}

_bool CPlayer::SetStamina(_float fValue)
{
	if (m_fStamina <= 0.f)
		return false;

	//인자로 들어온 값으로 비율을 구해서 넘겨줄거임.
	m_fStamina -= fValue;
	if (m_fStamina <= 0.f)
	{
		//0보다 작으면 UI매니저에 0던짐
		m_fStamina = 0.f;
		CUI_Manager::Get_Instance()->SetValue("StaminaBar", m_fStamina);
	}
	else
	{
		_float fCurrentRatio = m_fStamina / m_fMaxStamina;
		CUI_Manager::Get_Instance()->SetValue("StaminaBar", fCurrentRatio);
	}

	return true;
}

void CPlayer::IncreaseStamina(_float fTimeDelta, _float fIncreaseSpeed)
{
	m_fStamina += fTimeDelta* fIncreaseSpeed;
	if (m_fStamina >= m_fMaxStamina)
	{
		m_fStamina = m_fMaxStamina;
	}
	_float fCurrentRatio = m_fStamina / m_fMaxStamina;
	CUI_Manager::Get_Instance()->SetValue("StaminaBar", fCurrentRatio);
}


void CPlayer::OnCollisionEnter(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CPlayer::OnCollisionStay(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

void CPlayer::OnCollisionExit(CGameObject * pOther, _float fTimeDelta)
{
	int a = 10;
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
		
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fiona"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_AABB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB */	
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	/*ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);*/
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_OBB], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_SPHERE"), (CComponent**)&m_pColliderCom[COLLIDERTYPE_SPHERE], &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, L"Layer_Camera", L"Com_Transform", 0);
	//m_pCamera = (CCamera_Free*)pTransform->GetOwner();
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Ready_Sockets()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode*		pWeaponSocket = m_pModelCom->Get_HierarchyNode("Bone_sword");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Sword */
	CGameObject*		pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"));

	if (nullptr == pGameObject)
		return E_FAIL;

	((CSword*)pGameObject)->SetPlayer(this);
	m_Parts.push_back(pGameObject);

	return S_OK;
}

HRESULT CPlayer::Update_Weapon()
{
	if (nullptr == m_Sockets[PART_WEAPON])
		return E_FAIL;

	/* 행렬. */
	/*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
		* 뼈의 행렬(CombinedTransformation) 
		* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	_matrix WeaponMatrix = /*m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
			**/ m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
			* m_pModelCom->Get_PivotMatrix() 
			* m_pTransformCom->Get_WorldMatrix();

	m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);
	return S_OK;
}

_float3 CPlayer::GetNormalizeDir(_uint eState)
{
	_float3 vDir;
	XMStoreFloat3(&vDir, m_pTransformCom->Get_State((CTransform::STATE)eState));
	XMStoreFloat3(&vDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
	vDir.y = 0.0f;
	return vDir;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);

	if (nullptr != m_pTargetTransform)
		Safe_Release(m_pTargetTransform);

	Safe_Release(m_pCamera);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
