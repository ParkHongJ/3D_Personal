#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CAspiration final : public CGameObject
{
private:
	CAspiration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAspiration(const CAspiration& rhs);
	virtual ~CAspiration() = default;

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
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_float3					m_vStartScale;// 0.035
	_float3					m_vMaxScale;// 0.04

	_float					m_fDuration = 2.5f;
	_float					m_fCurrentTime = 0.0f;
	_float					m_fTime = 0.f;
	_bool					bTest = false;
private:
	HRESULT Ready_Components();

public:
	static CAspiration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END