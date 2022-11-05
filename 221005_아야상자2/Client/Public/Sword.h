#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CSword final : public CGameObject
{
private:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSword(const CSword& rhs);
	virtual ~CSword() = default;

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
	_float GetDamage() {
		return m_fDamage;
	}
	void SetPlayer(class CPlayer* pPlayer);
private:
	_bool m_bEnable = true;
private:
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	class CPlayer*			m_pPlayer = nullptr;

	_float					m_fDamage = 25.f;
private:
	HRESULT Ready_Components();

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END