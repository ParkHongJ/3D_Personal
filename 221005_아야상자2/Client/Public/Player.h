#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;

class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PARTTYPE { PART_WEAPON, PART_END };
	enum COLLIDERTYPE { COLLIDERTYPE_AABB, COLLIDERTYPE_OBB, COLLIDERTYPE_SPHERE, COLLILDERTYPE_END };
	enum STATE_PLAYER { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_ATTACK, STATE_JUMP, STATE_END };
	enum STATE_ANIM { Backstab_Deb_1, Backstab_Deb_2, Backstab_Deb_3, Backstab_Fin, Backstab_Fin_inter, 
		DashAir, DashBack, DashFront, DashLeft, Death, DeathLong, DoubleJumpCloth_Start, Health, 
		IdleFight, IdlePeace, JumpCloth_Air, JumpCloth_Land, JumpCloth_Start, LowFight1, Magic,
		Magic_001, Magic_2, Magic_3, Magic_4, Magic_5, Parry, Parry_2, Parry_3, Projection, ProjectionInAir,
		ProjectionLand, RolL2, RollAttack, RollAttack_2, RollAttack_3, RollAttack_4, RollAttack_5, 
		Run, Sleeping, SleepingStandUp, Slide, Sprint, Stun, Walk, WalkBack, WalkLeft, WalkRight,
		fight_coup1, fight_coup2, fight_coup1bis, fight_coup3, fight_coup4, fight_coup5, fight_coup6,
		fight_deb, fight_fin, fight_inter, fight_prep2, dash_air_v3, dash_right_v2, interaction_v2, interaction_v2_fin,
		power_fight_01, power_fight_02, power_fight_03, power_fight_04, power_fight_05, use_object_v3, walkfront, 
		Jump6_Land, Jump6_Start, attaquePiquee_Land, attaquePiquee_v3, CoupFaible1_fin, CoupFaible1_frappe1,
		CoupFaible1_frappe2, CoupFaible1_pause, CoupFaible1_prepa1, CoupFaible1_prepa2, CoupFaible2_fin,
		CoupFaible2_frappe1, CoupFaible2_frappe2, CoupFaible2_pause, CoupFaible2_prepa1, CoupFaible2_prepa2,
		CoupFaibleCharge_v3, CoupFaibleCharge_v3_01, CoupFaibleCharge_v3_02, CoupFaibleCharge_v3_03, CoupFaibleCharge_v3_04,
		CoupPuissant_v5_1, CoupPuissant_v5_2, CoupPuissant_v5_3, CoupPuissant_v5_4, CoupPuissant_v5_5, HitFail,
		ANIM_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	//State
	void SetState(STATE_ANIM eState, _float fTimeDelta);

	void Idle_State(_float fTimeDelta);
	void Run_State(_float fTimeDelta);
	void Idle_Fight_State(_float fTimeDelta);
	void Sprint_State(_float fTimeDelta);
	void Parring_State(_float fTimeDelta);

	void Jump(_float fTimeDelta);
	void MoveControl(_float fTimeDelta);

public:
	HRESULT Set_Camera(class CCamera_Free* pCamera);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	CNavigation*			m_pNavigationCom = nullptr;

private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;

private:
	STATE_PLAYER			m_eCurrentState = STATE_END;
	_bool					m_bAnimEnd = false;
	STATE_ANIM				m_eCurrentAnimState = ANIM_END;
	_bool					m_bSprint = false;
	_bool					m_bComboAttack = false;
	//입력이 없을경우 Idle로 돌아가는 변수
	_float					m_fBehaviorTimeMax = 2.5f;
	_float					m_fBehaviorTimeCurrent = 0.f;
	_bool					m_bParry = false;
	_float					m_fSpeed = 3.f;
	_float					m_fRotationSpeed = 7.f;

	//Jump
	_bool					m_bJumping = false;
	_float					m_fJumpPower = 5.f;
	_float					m_fJumpTime = 0.0f;
	_float					m_fGravity = 9.8f;
	_float					m_fPosY = 0.0f;

	//Roll
	_float					m_fRollTimeMax  = 1.3f;
	_float					m_fRollTime = 0.f;
	_bool					m_bWeaponEnable = false;
	
	//Camera
	class CCamera_Free*		m_pCamera = nullptr;
	_bool					m_bLockOn = false;
public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta)override;

private:
	HRESULT Ready_Components();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();

	HRESULT Update_Weapon();
	_float3 GetNormalizeDir(_uint eState);
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END