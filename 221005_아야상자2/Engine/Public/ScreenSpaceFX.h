#pragma once
#include "Base.h"

BEGIN(Engine)

class CScreenSpaceFX final : public CBase
{
private:
	CScreenSpaceFX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CScreenSpaceFX() = default;
public:
	void SetParameters(_uint iSampRadius, _float fRadius) { m_iSSAOSampRadius = iSampRadius; m_fRadius = fRadius; }
	void GetParameters(_uint &iSampRadius, _float &fRadius)
	{
		iSampRadius = m_iSSAOSampRadius;
		fRadius = m_fRadius;
	}
private:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY);

	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	_uint m_nWidth;
	_uint m_nHeight;

	// 0 ~ 20
	_uint m_iSSAOSampRadius = 10;
	// 0 ~ 50
	_float m_fRadius = 13.f;

	typedef struct
	{
		UINT nWidth;
		UINT nHeight;
		float fHorResRcp;
		float fVerResRcp;
		_float4 ProjParams;
		_float4x4 ViewMatrix;
		float fOffsetRadius;
		float fRadius;
		float fMaxDepth;
		UINT pad;
	} TDownscaleCB;
	ID3D11Buffer* m_pDownscaleCB;

	// SSAO values for usage with the directional light
	ID3D11Texture2D* m_pSSAO_RT;
	ID3D11UnorderedAccessView* m_pSSAO_UAV;
	ID3D11ShaderResourceView* m_pSSAO_SRV;

	// Downscaled depth buffer (1/4 size)
	ID3D11Buffer* m_pMiniDepthBuffer;
	ID3D11UnorderedAccessView* m_pMiniDepthUAV;
	ID3D11ShaderResourceView* m_pMiniDepthSRV;

	// Shaders
	ID3D11ComputeShader* m_pDepthDownscaleCS;
	ID3D11ComputeShader* m_pComputeCS;

	HRESULT Ready_Shaders();

public:
	void Compute(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV);
	void SetParameters(int iSSAOSampRadius, float fRadius) { m_iSSAOSampRadius = iSSAOSampRadius; m_fRadius = fRadius; }
	ID3D11ShaderResourceView* GetSSAOSRV() { return m_pSSAO_SRV; }
	ID3D11ShaderResourceView* GetMiniDepthSRV() { return m_pSSAO_SRV; }

private:
	void DownscaleDepth(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV);
	void ComputeSSAO();
	void Blur();

public:
	static CScreenSpaceFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY);
	virtual void Free() override;
};
END