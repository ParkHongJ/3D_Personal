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
	//투사체 발사하는손
	enum STATE_ANIM {
		HAND_DEATH, HAND_IDLE, HAND_PATTERN3, HAND_END
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

	_float3					m_vOffsetPosition = { 0.f,0.f,0.f };
	_float3					m_vOffsetAttack = { 0.f,0.f,0.f };

	_uint					m_iProjectileCount = 0;

	const _uint				m_iProjectileCountMax = 3;
	_float					m_fCurrentBulletTime = 0.0f;
	_float					m_fCurrentDelay = 0.0f;
	_float					m_fDelayMax = 2.f;

	//test
	STATE_ANIM							m_eNextState = HAND_END;
	_bool								m_bPatternEnd = false;

	//Effect Test
	_uint								m_iPass = 1;
	_float								m_fCut = 1.0f;
	_float								m_fDissolveSpeed = 2.f;
	_bool								m_bDissolve = false;
	_bool								m_bRender = true;
public:
	void Set_State(STATE_ANIM eState, _float fTimeDelta);

	void Set_Death();
	void SetRas_Samrah(class CTransform * pRasTransform);
	void Set_Target(class CTransform* pTarget);
	void Set_Pattern(STATE_ANIM eState);
	//본체로부터 얼마나 떨어진 위치에 있을것인가?
	void Set_OffsetPos(class CTransform* pRasTransform);
	void MoveToOffsetIdle();
	void MoveToOffsetAttack();
	HRESULT Ready_Layer_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);
protected:
	HRESULT Ready_Components();
	
public:
	static CRas_Hands3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END