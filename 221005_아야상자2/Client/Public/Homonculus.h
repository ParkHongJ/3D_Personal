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
END

BEGIN(Client)

class CHomonculus final : public CGameObject
{
public:
	enum ANIM_STATE{ Attack_02_fin, Attack_02_frappe, Attack_03, Big_Combo, Death_01, Death_02, Explode, GetUp, Hit01, Idle, PostCritic, Strafe_Left, Walk, Stun, STATE_END };
private:
	CHomonculus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHomonculus(const CHomonculus& rhs);
	virtual ~CHomonculus() = default;

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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	ANIM_STATE				m_eState = STATE_END;
	CTransform*				m_pTarget = nullptr;
	_float					m_fHp = 25.f;
	const _float			m_fMaxHp = 25.f;
	_bool					m_bAnimEnd = false;

public:
	void SetNaviIndex(_uint iIndex);
	void Set_State(ANIM_STATE eState, _float fTimeDelta);
	void GetDamaged(_float fDamage);
	void WalkState(_float fTimeDelta);


	void CreateExplosion();
private:
	HRESULT Ready_Components();

public:
	static CHomonculus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END