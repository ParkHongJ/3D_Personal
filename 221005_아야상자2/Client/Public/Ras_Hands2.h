#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CRas_Hands2 final : public CGameObject
{
public:
	//몬스터 소환하는손
	enum STATE_ANIM {
		HAND_DEATH, HAND_IDLE, HAND_CINEMATIC1, HAND_CINEMATIC2, HAND_PATTERN2, HAND_END
	};
public:
	typedef struct SpawnInfo {
		class CTransform*		pTarget = nullptr;
		_float3					vPos;
		_uint					iCellIndex;
	}SPAWN_INFO;
private:
	CRas_Hands2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRas_Hands2(const CRas_Hands2& rhs);
	virtual ~CRas_Hands2() = default;

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

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	_bool					m_bAnimEnd = false;
	class CTransform*		m_pRasTransform = nullptr;
	class CTransform*		m_pTarget = nullptr;
	STATE_ANIM				m_eState = HAND_END;
	_bool					m_bEnabled = false;
	_bool					m_bHit = false;

	_float					m_fCurrentChaseTime = 0.0f;
	_float					m_fChaseTimeMax = 3.8f;

	_float3					m_vOffsetPosition = { 0.f,0.f,0.f };
	_float3					m_vOffsetAttack = { 0.f,0.f,0.f };


	vector<_uint>			m_iNaviIndices;
public:
	void Set_State(STATE_ANIM eState, _float fTimeDelta);
	void SetRas_Samrah(class CTransform * pRasTransform);
	void Set_Target(class CTransform* pTarget);
	void Set_Pattern(STATE_ANIM eState);
	//본체로부터 얼마나 떨어진 위치에 있을것인가?
	void Set_OffsetPos(class CTransform* pRasTransform);
	void MoveToOffsetIdle();
	void MoveToOffsetAttack();
	void Set_Death();
protected:
	HRESULT Ready_Components();

public:
	static CRas_Hands2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END