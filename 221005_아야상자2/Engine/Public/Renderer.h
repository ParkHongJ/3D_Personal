#pragma once

#include "Component.h"

/* 화면에 그려져야할 객체들을 그려져야할 순서대로 모아서 보관한다.  */
/* 보관하고 있는 개첻르의 렌더함수를 호출해주낟. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public: /* 그려지는 순서에 따른 정의 */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_EFFECT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };
	enum SHADERTYPE { SHADER_DEFERRED, SHADER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CRenderer& rhs);
	virtual ~CRenderer() = default;



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw();

#ifdef _DEBUG
public:
	void SetParameters(_float fMiddleGrey, _float fWhite, _float fBloomThreshold, _float fBloomScale);
	void GetParameters(_float &fMiddleGrey, _float &fWhite, _float& fBloomThreshold, _float& fBloomScale);

public:
	void SetParameters(_uint iSampRadius, _float fRadius, _bool bSSao);
	void GetParameters(_uint &iSampRadius, _float &fRadius, _bool bSSao);

	HRESULT Add_DebugGroup(class CComponent* pDebugCom);
	void SetPostProcessing(_bool bEnable) {
		m_bEnable = bEnable;
	};
#endif // _DEBUG
private:
	list<class CGameObject*>				m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>		RENDEROBJECTS;
#ifdef _DEBUG
private:
	list<class CComponent*>					m_DebugObject;
#endif // _DEBUG

private:
	class CTarget_Manager*					m_pTarget_Manager = nullptr;
	class CLight_Manager*					m_pLight_Manager = nullptr;
	
	_bool									m_bEnable = true;
private:
	class CShader*							m_pShader[SHADER_END] = { nullptr };
	_float4x4								m_ViewMatrix, m_ProjMatrix;
#ifdef _DEBUG
	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
#endif // _DEBUG
	class CPostFX*							m_pPostFX = nullptr;
	class CScreenSpaceFX*					m_pScreenFX = nullptr;
	class CSSLR_Manager*					m_pSSLR = nullptr;

	// HDR light accumulation buffer
	ID3D11Texture2D* g_pHDRTexture = nullptr;
	ID3D11RenderTargetView* g_HDRRTV = nullptr;
	ID3D11ShaderResourceView* g_HDRSRV = nullptr;
private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_EffectBlend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_Effect();
	HRESULT Render_UI();
	HRESULT Render_PostProcessing();
	HRESULT Compute_SSAO();
	void ResetSRV();
#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END