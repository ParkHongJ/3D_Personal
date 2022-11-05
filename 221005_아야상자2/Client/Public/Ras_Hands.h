#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRas_Hands final : public CGameObject
{
public:
	enum STATE_ANIM {
		HAND_AOE1, HAND_AOE2, HAND_AOE2001, HAND_AOE3, HAND_FIRST_CLOSED, HAND_SLAM_FLY, HAND_DEATH, HAND_IDLE, HAND_END
	};
	enum COLLIDERTYPE { COLLIDERTYPE_OBB, COLLIDERTYPE_SPHERE, COLLILDERTYPE_END };
private:
	CRas_Hands(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRas_Hands(const CRas_Hands& rhs);
	virtual ~CRas_Hands() = default;

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
	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };
	CTexture*				m_pTextureCom = nullptr;

private:
	_bool					m_bAnimEnd = false;
	class CTransform*		m_pRasTransform = nullptr;
	class CTransform*		m_pTarget = nullptr;
	STATE_ANIM				m_eState = HAND_END;
	_bool					m_bAttackEnabled = false;
	_bool					m_bHitEnabled  = false;
	_bool					m_bHitDelay = false;
	_float					m_fCurrentDelayTime = 0.0f;
	_float					m_fMaxDelayTime = 0.2f;

	_float					m_fSpeed = 3.f;
	_bool					m_bChase = false;
	_float					m_fCurrentChaseTime = 0.0f;
	_float					m_fChaseTimeMax = 3.5f;

	_float					m_fAttackTime = 0.0f;
	_float					m_fAttackTimeMax = 2.5f;



	//Effect Test
	_uint					m_iPass = 1;
	_float					m_fCut = 1.0f;
	_float					m_fDissolveSpeed = 2.f;
	_bool					m_bDissolve = false;
	_bool					m_bRender = false;;
public:
	void SetRas_Samrah(class CTransform * pRasTransform);
	void Set_Target(class CTransform* pTarget);
	void Set_Pattern(STATE_ANIM eState);
	void Set_State(STATE_ANIM eState, _float fTimeDelta);
	void Set_Death();

protected:
	HRESULT Ready_Components();

public:
	static CRas_Hands* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END