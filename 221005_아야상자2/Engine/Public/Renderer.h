#pragma once

#include "Component.h"

/* 화면에 그려져야할 객체들을 그려져야할 순서대로 모아서 보관한다.  */
/* 보관하고 있는 개첻르의 렌더함수를 호출해주낟. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public: /* 그려지는 순서에 따른 정의 */
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };
	enum SHADERTYPE { SHADER_DEFERRED, SHADER_SSAO, SHADER_LUMINANCE, SHADER_HDR, SHADER_BLURX, SHADER_BLURY, SHADER_BLOOM, SHADER_TEST, SHADER_END };

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
	HRESULT Add_DebugGroup(class CComponent* pDebugCom);
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


private:
	class CShader*							m_pShader[SHADER_END] = { nullptr };
	_float4x4								m_ViewMatrix, m_ProjMatrix;
#ifdef _DEBUG
	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_SSAO();
	HRESULT Render_BlurX();
	HRESULT Render_BlurY();
	HRESULT Render_Bloom();
	HRESULT Render_BloomTest();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();
	HRESULT Render_HDR();
	HRESULT Render_LUMINANCE();
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