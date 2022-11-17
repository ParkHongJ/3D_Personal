#pragma once
#include "Base.h"

BEGIN(Engine)

class CPostFX final : public CBase
{
private:
	CPostFX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CPostFX() = default;

	HRESULT Initialize(_uint iSizeX, _uint iSizeY);

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
	HRESULT Ready_DownScale();
	HRESULT Ready_Shaders();
	// Down scale the full size HDR image
	void DownScale(ID3D11ShaderResourceView* pHDRSRV);

	void Bloom();
	void Blur(ID3D11ShaderResourceView* pInput, ID3D11UnorderedAccessView* pOutput);

	// Final pass that composites all the post processing calculations
	void FinalPass(ID3D11ShaderResourceView* pHDRSRV);


	// ´Ù¿î½ºÄÉÀÏµÈ ¾ÀÀå¸éÀ» ÀúÀåÇÒ ·»´õÅ¸°Ù
	ID3D11Texture2D* m_pDownScaleRT = nullptr;
	ID3D11ShaderResourceView* m_pDownScaleSRV = nullptr;
	ID3D11UnorderedAccessView* m_pDownScaleUAV = nullptr;

	// ÀÓ½Ã·Î ÀúÀåÇØ³õÀ» ·»´õÅ¸°Ù
	ID3D11Texture2D* m_pTempRT[2];
	ID3D11ShaderResourceView* m_pTempSRV[2];
	ID3D11UnorderedAccessView* m_pTempUAV[2];

	// ºí·ë ·»´õÅ¸°Ù
	ID3D11Texture2D* m_pBloomRT = nullptr;
	ID3D11ShaderResourceView* m_pBloomSRV = nullptr;
	ID3D11UnorderedAccessView* m_pBloomUAV = nullptr;


	// 1D intermediate storage for the down scale operation
	ID3D11Buffer* m_pDownScale1DBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pDownScale1DUAV = nullptr;
	ID3D11ShaderResourceView* m_pDownScale1DSRV = nullptr;

	// Æò±ÕÈÖµµ¸¦ ÀúÀåÇÒSRV
	ID3D11Buffer* m_pAvgLumBuffer = nullptr;
	ID3D11UnorderedAccessView* m_pAvgLumUAV = nullptr;
	ID3D11ShaderResourceView* m_pAvgLumSRV = nullptr;

	_uint m_nWidth;
	_uint m_nHeight;
	_uint m_nDownScaleGroups;
	_float m_fMiddleGrey;
	_float m_fWhite;
	_float m_fAdaptation;
	_float m_fBloomThreshold;
	_float m_fBloomScale;

	typedef struct
	{
		_uint nWidth;
		_uint nHeight;
		_uint nTotalPixels;
		_uint nGroupSize;
		float fAdaptation;
		float fBloomThreshold;
		UINT pad[2];
	} TDownScaleCB;

	typedef struct
	{
		_float fMiddleGrey;
		_float fLumWhiteSqr;
		_float fBloomScale;
		_uint pad;
	} TFinalPassCB;

	typedef struct
	{
		UINT numApproxPasses;
		float fHalfBoxFilterWidth;			// w/2
		float fFracHalfBoxFilterWidth;		// frac(w/2+0.5)
		float fInvFracHalfBoxFilterWidth;	// 1-frac(w/2+0.5)
		float fRcpBoxFilterWidth;			// 1/w
		UINT pad[3];
	} TBlurCB;

	// Buffers
	ID3D11Buffer*				m_pDownScaleCB = nullptr;
	ID3D11Buffer*				m_pFinalPassCB = nullptr;
	ID3D11Buffer*				m_pBlurCB = nullptr;

	// Shaders
	ID3D11ComputeShader*		m_pDownScaleFirstPassCS = nullptr;
	ID3D11ComputeShader*		m_pDownScaleSecondPassCS = nullptr;
	ID3D11VertexShader*			m_pFullScreenQuadVS = nullptr;
	ID3D11PixelShader*			m_pFinalPassPS = nullptr;
	ID3D11ComputeShader*		m_pBloomRevealCS = nullptr;
	ID3D11ComputeShader*		m_HorizontalBlurCS = nullptr;
	ID3D11ComputeShader*		m_VerticalBlurCS = nullptr;

	// Previous average luminance for adaptation
	ID3D11Buffer* m_pPrevAvgLumBuffer;
	ID3D11UnorderedAccessView* m_pPrevAvgLumUAV;
	ID3D11ShaderResourceView* m_pPrevAvgLumSRV;

	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	ID3D11SamplerState*         m_pSampPoint = nullptr;
	ID3D11SamplerState*         m_pSampLinear = nullptr;


public:
	static CPostFX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	virtual void Free() override;
};
END