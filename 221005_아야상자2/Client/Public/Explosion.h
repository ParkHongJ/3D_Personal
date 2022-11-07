#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CExplosion final : public CGameObject
{
private:
	CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosion(const CExplosion& rhs);
	virtual ~CExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta)override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta)override;

private:
	CShader*				m_pShaderCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
private:
	_uint					m_iPass = 0;
	_float					m_fDissolveSpeed = 1.0f;
	_float					m_fCut = 0.0f;

	_float3					m_vScale = { 0.03f, 0.03f, 0.03f };
	_float					m_fExplosionSpeed = 2.5f;

	_float					m_fDuration = 0.25f;
	_float					m_fCurrentTime = 0.0f;

	_bool					m_bDissolve = false;
private:
	HRESULT Ready_Components();

public:
	static CExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END