#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
class CTransform;
class CModel;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)
class CYantari final : public CGameObject
{
public:
	enum PARTTYPE { PART_WEAPON, PART_END };
public:
	enum ANIM_STATE {
		ATTACK1_2, ATTACK1_3, ATTACK1_4, ATTACK1_6, ATTACK1_7, ATTACK1_8, ATTACK1_9, 
		ATTACK2_2, ATTACK2_3, ATTACK2_4, ATTACK2_5, ATTACK2_6, ATTACK2_END, ATTACK2_START, ATTACK3, 
		ATTACK3_2, ATTACK3_3, ATTACK3_4, ATTACK3_5, ATTACK3_6, ATTACK3_7, ATTACK3_8, 
		ATTACK4_2, ATTACK4_3, ATTACK4_4, ATTACK4_5, ATTACK4_6, ATTACK4_7, 
		ATTACK5_BIS, ATTACK5_BIS2, CAST, DEATH, GETUP, HIT, HIT_DEBUT, HIT_FIN, IDLE, POST_CRITIC,
		WALK_BACK, WALK_FRONT, WALK_LEFT, WALK_RIGHT, ANIM_END

		//Attack1 : �����
		//Attack2 : Ⱦ����
		//Attack3 : �ѹ� �����ϰ� �������
		//Attack4 : �����ϸ� ���
	};
private:
	CYantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYantari(const CYantari& rhs);
	virtual ~CYantari() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta) override;

public:
	HRESULT Ready_Layer_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);
	void GetDamage(_float fDamage);

	void Parried();
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CTransform*				m_pTargetTransform = nullptr;

private:
	vector<class CHierarchyNode*>		m_Sockets;
	_float								m_fHp = 1000;
	_bool								m_bAnimEnd = false;
	_float								m_fSpeed = 40.f;
	_float								m_fRotationSpeed = 7.f;
	_float								m_fDashSpeed = 35.f;
	_float								m_fMoveSpeed = 3.f;
	//�޺�����.
	//���� ���° �޺�����, �ִ��޺���
	_uint								m_iCurrentCombo = 0;
	const _uint							m_iMaxCombo = 4;


	//�ǰ� ���ú���
	_bool								m_bHitEnabled = true; // ���� �� �ִ���
	_bool								m_bHitDelay = false; // ������ �ٰ���
	_float								m_fCurrentHitDelayTime = 0.0f; //���� �ð�
	_float								m_fMaxHitDelayTime = 0.2f; //�ǰ� �����̽ð�

	//�޺��� ������ ���½ð��� �ִ뽬�½ð�
	_float								m_fCurrentDelayTime = 0.0f;
	const _float						m_fMaxDelayTime = 3.0f;

	//��ų�� ��ų������ ������
	_float								m_fCurrentGlobalDelayTime = 0.0f;
	const _float						m_fGlobalMaxDelayTime = 0.15f;

	//������ ��Ÿ��
	_bool								m_bCanDashAttack = true; // ������ ��� �� �� �ִٸ� True.
	_float								m_fCurrentDashAttackTime = 0.0f;
	const _float						m_fMaxDashAttackTime = 5.f;

	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;
	
	//���� Ȱ��ȭ
	_bool								m_bPartsEnable = false;

	ANIM_STATE							m_eAnimState = ANIM_END;

public:
	void Set_State(ANIM_STATE eState, _float fTimeDelta);

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();

	HRESULT Update_Weapon();
	HRESULT Ready_Components();
public:
	static CYantari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END