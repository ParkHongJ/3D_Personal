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

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	CGameObject*						m_Parts = nullptr;
	vector<class CHierarchyNode*>		m_Sockets;
	_float								m_fHp = 100;

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
	class CTransform*					m_pTargetTransform = nullptr;

	//패턴3 투사체발사할때 기준점
	_float3								m_OffsetProjectile;
	
	//핸드 1은 내려치기밖에없고 오프셋이 없음.
	_float3								m_OffsetHands2;
	_float3								m_OffsetHands3;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();

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