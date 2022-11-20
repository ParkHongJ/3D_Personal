#pragma once
#include "Base.h"

BEGIN(Engine)

class CSSLR_Manager final : public CBase
{
private:
	CSSLR_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CSSLR_Manager() = default;

private:
#pragma pack(push,1)
	struct CB_OCCLUSSION
	{
		UINT nWidth;
		UINT nHeight;
		UINT pad[2];
	};

	struct CB_LIGHT_RAYS
	{
		_float2 vSunPos;
		float fInitDecay;
		float fDistDecay;
		_float3 vRayColor;
		float fMaxDeltaLen;
	};
#pragma pack(pop)
	HRESULT Initialize(_uint iSizeX, _uint iSizeY);

	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	_uint m_nWidth;
	_uint m_nHeight;
	float m_fInitDecay;
	float m_fDistDecay;
	float m_fMaxDeltaLen;

	ID3D11Texture2D* m_pOcclusionTex = nullptr;
	ID3D11UnorderedAccessView* m_pOcclusionUAV = nullptr;
	ID3D11ShaderResourceView* m_pOcclusionSRV = nullptr;

	ID3D11Texture2D* m_pLightRaysTex = nullptr;
	ID3D11RenderTargetView* m_pLightRaysRTV = nullptr;
	ID3D11ShaderResourceView* m_pLightRaysSRV = nullptr;

	// Shaders
	ID3D11Buffer* m_pOcclusionCB = nullptr;
	ID3D11ComputeShader* m_pOcclusionCS = nullptr;
	ID3D11Buffer* m_pRayTraceCB = nullptr;
	ID3D11VertexShader* m_pFullScreenVS = nullptr;
	ID3D11PixelShader* m_pRayTracePS = nullptr;
	ID3D11PixelShader* m_pCombinePS = nullptr;

	// Additive blend state to add the light rays on top of the scene lights
	ID3D11BlendState* m_pAdditiveBlendState;

	HRESULT Ready_Shaders();
public:
	// Prepare the occlusion map
	void PrepareOcclusion(ID3D11ShaderResourceView* pMiniDepthSRV);

	// Ray trace the occlusion map to generate the rays
	void RayTrace(const _float2& vSunPosSS, const _float3& vSunColor);

	// Combine the rays with the scene
	void Combine(ID3D11RenderTargetView* pLightAccumRTV);

	// Render the screen space light rays on top of the scene
	void Render(ID3D11RenderTargetView* pLightAccumRTV, ID3D11ShaderResourceView* pMiniDepthSRV, const _float3& vSunDir, const _float3& vSunColor);

public:
	static CSSLR_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY);
	virtual void Free() override;
};
END