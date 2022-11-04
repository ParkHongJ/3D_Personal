#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Cell.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CModel;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)

class CRas_Samrah final : public CGameObject
{
public:
	enum PARTTYPE { PART_WEAPON, PART_END };

	enum STATE_ANIM {
		Jug_FlyHit1,
		Jug_FlyHit2,
		HitPhase2,
		RigAnim,
		SpawnHammer,
		Death,
		Fly,
		Idle1,
		Idle2,
		Pattern1,
		Pattern3,
		WalkGround,
		ANIM_END
	};
	enum PHASE {
		PHASE_1,
		PHASE_2,
		PHASE_END
	};
	enum Hands {
		HAND_1,
		HAND_2,
		HAND_3,
		HAND_END
	};
private:
	CRas_Samrah(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRas_Samrah(const CRas_Samrah& rhs);
	virtual ~CRas_Samrah() = default;

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
	void SetNaviTypes();
	HRESULT Ready_Layer_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);
	void SetNaviTypes(CCell::CELLTYPE eType);
	const _float GetMaxHP() {
		return m_fMaxHP;
	}

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
private:
	CGameObject*						m_Parts = nullptr;
	vector<class CHierarchyNode*>		m_Sockets;
	const _float						m_fMaxHP = 100.f;
	_float								m_fHp = m_fMaxHP;

	STATE_ANIM							m_eCurrentAnimState = ANIM_END;
	PHASE								m_ePhase = PHASE_END;
	_bool								m_bAnimEnd = false;
	_bool								m_bActiveHammer = false;
	_float								m_fHammerSpawnTime = 0.0f;
	_float								m_fHammerSpawnMaxTime = 1.4f;
	_bool								m_bTimeCheck = false;

	class CRas_Hands*					m_pHand1 = nullptr;
	class CRas_Hands2*					m_pHand2 = nullptr;
	class CRas_Hands3*					m_pHand3 = nullptr;
	CTransform*							m_pTargetTransform = nullptr;
	
	//패턴3 투사체발사할때 기준점
	_float3								m_OffsetProjectile;
	
	//핸드 1은 내려치기밖에없고 오프셋이 없음.
	_float3								m_OffsetHands2;
	_float3								m_OffsetHands3;

	//길막는용
	vector<_uint>						m_iNaviIndices;
	//물기둥소환용
	vector<_uint>						m_iCylinderIndices;

	_float3								m_vOffsetPattern;

	_float								m_fPatternDelay = 0.0f;

	_float								m_fPatternMaxDelay = 9.0f;
	_bool								m_bHand2 = false;
	
	//물기둥패턴
	_bool								m_bCylinder = false;
	_float								m_fCylinderTime = 0.0f;
	_float								m_fCylinderTimeMax = 0.7f;
	
	_uint								m_iCylinderCountMax = 7;
	_uint								m_iCylinderCount = 0;


	//해머스폰 이펙트
	_float								m_fCurrentEffectTime = 0.0f;
	const _float						m_fMaxEffectTime = 0.2f;
	_bool								m_bEffectEnable = false;

	//Effect Test
	_uint								m_iPass = 0;
	_float								m_fCut = 0.0f;
	_float								m_fDissolveSpeed = 3.f;
	_bool								m_bDissolve = false;
private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();
	HRESULT Ready_Projectile();
	HRESULT Ready_Cylinder();

	void Set_State(STATE_ANIM eAnim, PHASE ePhase, _float fTimeDelta);
	HRESULT Update_Weapon();
	HRESULT Ready_Components();
	HRESULT Ready_Hands();
public:
	static CRas_Samrah* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END