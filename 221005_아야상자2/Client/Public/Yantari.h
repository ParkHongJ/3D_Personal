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

		//Attack1 : 대어파
		//Attack2 : 횡베기
		//Attack3 : 한번 묵직하게 내려찍기
		//Attack4 : 돌진하며 찌르기
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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	vector<class CHierarchyNode*>		m_Sockets;
	_float								m_fHp = 100;
	_bool								m_bAnimEnd = false;
	_float								m_fSpeed = 40.f;

	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;
	ANIM_STATE							m_eAnimState = ANIM_END;

public:
	void Set_State(ANIM_STATE eState);
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