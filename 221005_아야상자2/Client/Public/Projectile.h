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

class CProjectile final : public CGameObject
{
public:
	enum PHASE {
		//PHASE1 : 그냥 총알처럼 발사
		//PHASE2 : 공전후에 발사.
		PHASE1, PHASE2, PHASE_END
	};
	typedef struct ProjectileInfo {
		_float3 vPos;
		_float3 vDir;
		_float  fLimitY;
		PHASE	ePhase;
		_float3 vOffset; //공전용 변수. 어디를 기준으로 돌건지
		_float  fDelayTime;
	}PROJECTILEINFO;
private:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void SetDir(_fvector vDir);
	void CreateExplosion();
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

	
private:
	//투사체 속도
	_float					m_fSpeed = 35.f;
	
	_float					m_fCurrentTime = 0.0f;
	//투사체 방향
	_float3					m_vDir;
	_bool					m_bReady = false;

	//원점으로부터의 거리
	_float3					m_vDistance;
	//폭발
	const _float			m_fMaxExplodeTime = 2.f;
	_float					m_fCurrentExplodeTime = 0.0f;
	PROJECTILEINFO			m_ProjInfo;
	
private:
	HRESULT Ready_Components();

public:
	static CProjectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END