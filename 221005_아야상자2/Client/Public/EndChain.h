#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CEndChain final : public CGameObject
{
	enum CHAIN_STATE { NORMAL, BREAKED, CHAIN_END};
private:
	CEndChain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEndChain(const CEndChain& rhs);
	virtual ~CEndChain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT SetUp_State(_fmatrix StateMatrix);

public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta)override;

public:
	void SetEnable(_bool bEnable) {
		m_bEnable = bEnable;
	}
	const _bool GetEanble() {
		return m_bEnable;
	}

	void Broken();
	void GetDamaged(_float fDamage);
private:
	_bool m_bEnable = true;
	_float m_fHp = 25.f;
	const _float m_fMaxHp = 25.f;

	_float3					m_vColor = { 0.8f, 0.8f, 0.8f };
private:
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CModel*					m_pModelCom[CHAIN_END] = { nullptr };
	class CRas_Samrah*		m_pRasSamrah = nullptr;

private:
	CHAIN_STATE m_eChain = CHAIN_END;


private:
	HRESULT Ready_Components();

public:
	static CEndChain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END