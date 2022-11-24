#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Point;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	enum DistortionType {
		DEFAULT = -1,
		SPREAD = 1
	};
	typedef struct Effect_Desc {
		_float4 vPosition;
		_float4 vScale;
		DistortionType	eSign; //모일건지 퍼질건지
	}EFFECT_DESC;
private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	//CVIBuffer_Point*		m_pVIBufferCom = nullptr;

	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float					m_fSpeed = 2.f;
	_uint					m_iNumTex = 15;
	_float					m_iCurrentTex = 0.f;
	_uint					m_iPass = 0;
	_float					m_fTime = 0.0f;

	EFFECT_DESC				m_tEffectDesc;

private:
	HRESULT Ready_Components();

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END