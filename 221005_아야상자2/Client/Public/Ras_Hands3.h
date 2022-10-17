#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRas_Hands3 final : public CGameObject
{
public:
	enum STATE_ANIM {
		HAND_AOE1, HAND_AOE2, HAND_AOE2001, HAND_AOE3, HAND_FIRST_CLOSED, HAND_SLAM_FLY, HAND_DEATH, HAND_IDLE, HAND_END
	};
	private:
	CRas_Hands3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRas_Hands3(const CRas_Hands3& rhs);
	virtual ~CRas_Hands3() = default;

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
	void GetDamaged(_float fDamage);

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_bool					m_bAnimEnd = false;
	class CTransform*		m_pRasTransform = nullptr;
	class CTransform*		m_pTarget = nullptr;
	STATE_ANIM				m_eState = HAND_END;
	_bool					m_bEnabled = false;
	_bool					m_bHit = false;

	_float					m_fSpeed = 3.f;
	_bool					m_bChase = false;
	_float					m_fCurrentChaseTime = 0.0f;
	_float					m_fChaseTimeMax = 3.5f;

	void Set_State(STATE_ANIM eState, _float fTimeDelta);


public:
	void SetRas_Samrah(class CTransform * pRasTransform);
	void Set_Target(class CTransform* pTarget);
	void Set_Pattern(STATE_ANIM eState);

protected:
	HRESULT Ready_Components();

public:
	static CRas_Hands3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END