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

public:
	// Entry point for post processing
	void PostProcessing(ID3D11ShaderResourceView* pHDRSRV);

	void SetParameters(float fMiddleGrey, float fWhite) { m_fMiddleGrey = fMiddleGrey; m_fWhite = fWhite; }
	void GetParameters(_float &fMiddleGrey, _float &fWhite)
	{
		fMiddleGrey = m_fMiddleGrey;
		fWhite = m_fWhite;
	}

private:

	// Down scale the full size HDR image
	void DownScale(ID3D11ShaderResourceView* pHDRSRV);

	// Final pass that composites all the post processing calculations
	void FinalPass(ID3D11ShaderResourceView* pHDRSRV);

	// 1D intermediate storage for the down scale operation
	ID3D11Buffer* m_pDownScale1DBuffer;
	ID3D11UnorderedAccessView* m_pDownScale1DUAV;
	ID3D11ShaderResourceView* m_pDownScale1DSRV;

	// Average luminance
	ID3D11Buffer* m_pAvgLumBuffer;
	ID3D11UnorderedAccessView* m_pAvgLumUAV;
	ID3D11ShaderResourceView* m_pAvgLumSRV;

	_uint m_nWidth;
	_uint m_nHeight;
	_uint m_nDownScaleGroups;
	_float m_fMiddleGrey;
	_float m_fWhite;

	typedef struct
	{
		_uint nWidth;
		_uint nHeight;
		_uint nTotalPixels;
		_uint nGroupSize;
	} TDownScaleCB;
	ID3D11Buffer* m_pDownScaleCB;

	typedef struct
	{
		_float fMiddleGrey;
		_float fLumWhiteSqr;
		_uint pad[2];
	} TFinalPassCB;

	ID3D11Buffer* m_pFinalPassCB;

	// Shaders
	ID3D11ComputeShader*		m_pDownScaleFirstPassCS = nullptr;
	ID3D11ComputeShader*		m_pDownScaleSecondPassCS = nullptr;
	ID3D11VertexShader*			m_pFullScreenQuadVS = nullptr;
	ID3D11PixelShader*			m_pFinalPassPS = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	ID3D11SamplerState*         g_pSampPoint = nullptr;


public:
	static CPostFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	virtual void Free() override;
};
END