#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11Texture2D* Get_Texture2D() const {
		return m_pTexture2D;
	}

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor, _bool bHDR, _float fNear = 0, _float fFar = 300.f);
	HRESULT Clear();
	HRESULT Bind_SRV(class CShader* pShader, const char* pConstantName);
	ID3D11ShaderResourceView* Get_SRV() { return m_pSRV; }
#ifdef _DEBUG
public:
	HRESULT Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
#endif // _DEBUG

#ifdef _DEBUG
public:
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer* pVIBuffer, _uint iPass = 0, const char* szName = "g_DiffuseTexture");
#endif // _DEBUG

	_matrix GetOrthoMatrix();

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	ID3D11Texture2D*			m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;

	_float4						m_vClearColor = _float4(0.f, 0.f, 0.f, 1.f);

	_float4x4					m_OrthoMatrix;
#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
#endif // _DEBUG

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor, _bool bHDR);
	virtual void Free() override;
};

END