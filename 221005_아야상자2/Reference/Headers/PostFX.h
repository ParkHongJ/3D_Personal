#pragma once
#include "Base.h"

BEGIN(Engine)

class CPostFX final : public CBase
{
private:
	CPostFX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CPostFX() = default;

	HRESULT Initialize(_uint iSizeX, _uint iSizeY);
	void Deinit();

	// Entry point for post processing
	void PostProcessing(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pHDRSRV, ID3D11RenderTargetView* pLDRRTV);

	void SetParameters(float fMiddleGrey, float fWhite) { m_fMiddleGrey = fMiddleGrey; m_fWhite = fWhite; }

private:

	// Down scale the full size HDR image
	void DownScale(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pHDRSRV);

	// Final pass that composites all the post processing calculations
	void FinalPass(ID3D11DeviceContext* pd3dImmediateContext, ID3D11ShaderResourceView* pHDRSRV);

	// 1D intermediate storage for the down scale operation
	ID3D11Buffer* m_pDownScale1DBuffer;
	ID3D11UnorderedAccessView* m_pDownScale1DUAV;
	ID3D11ShaderResourceView* m_pDownScale1DSRV;

	// Average luminance
	ID3D11Buffer* m_pAvgLumBuffer;
	ID3D11UnorderedAccessView* m_pAvgLumUAV;
	ID3D11ShaderResourceView* m_pAvgLumSRV;

	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nDownScaleGroups;
	float m_fMiddleGrey;
	float m_fWhite;

	typedef struct
	{
		UINT nWidth;
		UINT nHeight;
		UINT nTotalPixels;
		UINT nGroupSize;
	} TDownScaleCB;
	ID3D11Buffer* m_pDownScaleCB;

	typedef struct
	{
		float fMiddleGrey;
		float fLumWhiteSqr;
		UINT pad[2];
	} TFinalPassCB;
	ID3D11Buffer* m_pFinalPassCB;

	// Shaders
	ID3D11ComputeShader* m_pDownScaleFirstPassCS;
	ID3D11ComputeShader* m_pDownScaleSecondPassCS;
	ID3D11VertexShader* m_pFullScreenQuadVS;
	ID3D11PixelShader* m_pFinalPassPS;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	ID3D11SamplerState*         g_pSampPoint = nullptr;
public:
	static CPostFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	virtual void Free() override;
};
END