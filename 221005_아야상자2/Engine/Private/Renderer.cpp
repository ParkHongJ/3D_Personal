#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "PostFX.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	
	_uint iWidth = 1280;
	_uint iHeight = 720;

	m_pPostFX = CPostFX::Create(m_pDevice, m_pContext, iWidth, iHeight, DXGI_FORMAT_B4G4R4A4_UNORM, &_float4(0.f, 0.f, 0.f, 1.f));
	if (nullptr == m_pPostFX)
		return E_FAIL;
	

	_uint		iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	/* For.Target_Original */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Original"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 0.f), true)))
		return E_FAIL;

	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_SSAO */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_BlurX */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurX"), (_uint)ViewportDesc.Width / 2, (_uint)ViewportDesc.Height / 2, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_BlurY */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurY"), (_uint)ViewportDesc.Width / 2, (_uint)ViewportDesc.Height / 2, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_DownSampler */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_DownSampler"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_UpSampler */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_UpSampler"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_BlurUpScale */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BlurUpScale"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.0f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Bloom */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Bloom"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_BloomTest */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BloomTest"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_HDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

#pragma region Luminance
	/* For.Target_Luminance1 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance1"), 243, 243, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Luminance2 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance2"), 81, 81, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Luminance3 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance3"), 27, 27, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Luminance4 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance4"), 9, 9, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Luminance5 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance5"), 3, 3, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For.Target_Luminance6 */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Luminance6"), 1, 1, DXGI_FORMAT_R32G32_FLOAT, &_float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

#pragma endregion



	/* For.MRT_Original */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Original"), TEXT("Target_Original"))))
		return E_FAIL;

	/* For.MRT_Deferred */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* For.MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* For.MRT_SSAO */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;

	/* For.MRT_Blur */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurY"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BlurUpScale"), TEXT("Target_BlurUpScale"))))
		return E_FAIL;
	
	/* For.MRT_Bloom*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
		return E_FAIL;

	/* For.MRT_Bloom*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_BloomTest"), TEXT("Target_BloomTest"))))
		return E_FAIL;

	/* For.MRT_HDR */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;

	/* For.MRT_Luminance1 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance1"), TEXT("Target_Luminance1")))) // 243 243
		return E_FAIL;
	/* For.MRT_Luminance2 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance2"), TEXT("Target_Luminance2")))) // 81 81
		return E_FAIL;
	/* For.MRT_Luminance3 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance3"), TEXT("Target_Luminance3")))) // 27 27
		return E_FAIL;
	/* For.MRT_Luminance4 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance4"), TEXT("Target_Luminance4")))) // 9 9
		return E_FAIL;
	/* For.MRT_Luminance5 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance5"), TEXT("Target_Luminance5")))) // 3 3
		return E_FAIL;
	/* For.MRT_Luminance6 */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Luminance6"), TEXT("Target_Luminance6")))) // 1 1
		return E_FAIL;
#ifdef _DEBUG

	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Diffuse"), 50.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Normal"), 50.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Depth"), 50.f, 250.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Shade"), 150.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Specular"), 150.f, 150.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_SSAO"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurX"), 500.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurY"), 500.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Bloom"), 700.f, 100.f, 200.f, 200.f)))
		return E_FAIL; 
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BloomTest"), 900.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_BlurUpScale"), 700.f, 500.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance1"), 50.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance2"), 50.f, 450.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance3"), 50.f, 550.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance4"), 150.f, 350.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance5"), 150.f, 450.f, 100.f, 100.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Initialize_Debug(TEXT("Target_Luminance6"), 150.f, 550.f, 100.f, 100.f)))
		return E_FAIL;

	m_pShader[SHADER_DEFERRED] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_DEFERRED])
		return E_FAIL;

	m_pShader[SHADER_SSAO] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/SSAO.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_SSAO])
		return E_FAIL;

	m_pShader[SHADER_BLURX] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/BlurX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_BLURX])
		return E_FAIL;

	m_pShader[SHADER_BLURY] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/BlurY.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_BLURY])
		return E_FAIL;

	m_pShader[SHADER_BLOOM] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Bloom.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_BLOOM])
		return E_FAIL;

	m_pShader[SHADER_TEST] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/TEST.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_TEST])
		return E_FAIL;

	m_pShader[SHADER_LUMINANCE] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Luminance.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_LUMINANCE])
		return E_FAIL;

	m_pShader[SHADER_HDR] = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/HDR.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader[SHADER_HDR])
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

#endif // _DEBUG


	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);


	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_PostProcessing()))
		return E_FAIL;

	if (FAILED(Render_HDR()))
		return E_FAIL;

	if (FAILED(Render_LUMINANCE()))
		return E_FAIL;

	if (FAILED(Render_Bloom()))
		return E_FAIL;

	if (FAILED(Render_BlurX()))
		return E_FAIL;

	if (FAILED(Render_BlurY()))
		return E_FAIL;

	if (FAILED(Render_BloomTest()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


#ifdef _DEBUG

	if (FAILED(Render_Debug()))
		return E_FAIL;

#endif


	return S_OK;
}

#ifdef _DEBUG
void CRenderer::SetParameters(_float fMiddleGrey, _float fWhite)
{
	m_pPostFX->SetParameters(fMiddleGrey, fWhite);
}
void CRenderer::GetParameters(_float & fMiddleGrey, _float & fWhite)
{
	m_pPostFX->GetParameters(fMiddleGrey, fWhite);
}
HRESULT CRenderer::Add_DebugGroup(CComponent* pDebugCom)
{
	m_DebugObject.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif // DEBUG


HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4			ViewMatrixInv;
	_float4x4			ProjMatrixInv;

	XMStoreFloat4x4(&ViewMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrixInv, XMMatrixTranspose(pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ViewMatrixInv", &ViewMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ProjMatrixInv", &ProjMatrixInv, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Normal"), m_pShader[SHADER_DEFERRED], "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader[SHADER_DEFERRED], "g_DepthTexture")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_SSAO"), m_pShader[SHADER_DEFERRED], "g_SSAOTexture")))
	//	return E_FAIL;

	m_pLight_Manager->Render(m_pShader[SHADER_DEFERRED], m_pVIBuffer);

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	ID3D11ShaderResourceView*		pSRVs[8] =
	{
		nullptr
	};

	m_pContext->PSSetShaderResources(0, 8, pSRVs);

	return S_OK;
}


HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_SSAO"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_SSAO]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_SSAO]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_SSAO]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Normal"), m_pShader[SHADER_SSAO], "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Depth"), m_pShader[SHADER_SSAO], "g_DepthTexture")))
		return E_FAIL;

	m_pShader[SHADER_SSAO]->Begin(0);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurX()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurX"))))
		return E_FAIL;

		_float4x4			WorldMatrix;

		_uint				iNumViewport = 1;
		D3D11_VIEWPORT		ViewportDesc;

		m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

		XMStoreFloat4x4(&WorldMatrix,
			XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

		if (FAILED(m_pShader[SHADER_BLURX]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader[SHADER_BLURX]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader[SHADER_BLURX]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Bloom"), m_pShader[SHADER_BLURX], "g_BlurTexture")))
			return E_FAIL;


		m_pShader[SHADER_BLURX]->Begin(0);

#ifdef _DEBUG
		m_pVIBuffer->Render();
#endif
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurY()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BlurY"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_BLURY]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_BLURY]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_BLURY]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurX"), m_pShader[SHADER_BLURY], "g_BlurTexture")))
		return E_FAIL;

	m_pShader[SHADER_BLURY]->Begin(0);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Bloom"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_BLOOM]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_BLOOM]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_BLOOM]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Original"), m_pShader[SHADER_BLOOM], "g_BlurTexture")))
		return E_FAIL;

	m_pShader[SHADER_BLOOM]->Begin(0);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BloomTest()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BloomTest"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_TEST]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_TEST]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_TEST]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_BlurY"), m_pShader[SHADER_TEST], "g_BlurTest")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Original"), m_pShader[SHADER_TEST], "g_OriginalTexture")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Bloom"), m_pShader[SHADER_TEST], "g_BlurOriginTexture")))
		return E_FAIL;

	m_pShader[SHADER_TEST]->Begin(0);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Original"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Diffuse"), m_pShader[SHADER_DEFERRED], "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Shade"), m_pShader[SHADER_DEFERRED], "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Specular"), m_pShader[SHADER_DEFERRED], "g_SpecularTexture")))
		return E_FAIL;
	
	m_pShader[SHADER_DEFERRED]->Begin(3);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Diffuse"), m_pShader[SHADER_DEFERRED], "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Shade"), m_pShader[SHADER_DEFERRED], "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Specular"), m_pShader[SHADER_DEFERRED], "g_SpecularTexture")))
		return E_FAIL;

	m_pShader[SHADER_DEFERRED]->Begin(3);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	if (FAILED(m_pShader[SHADER_HDR]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_HDR]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_HDR]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_Original"), m_pShader[SHADER_HDR], "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader[SHADER_HDR]->Begin(0);

#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_LUMINANCE()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	/* Target_Shade타겟에 빛 연산한 결과를 그린다. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Luminance1"))))
		return E_FAIL;
	_float4x4			WorldMatrix;

	_uint				iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&WorldMatrix,
		XMMatrixTranspose(XMMatrixScaling(243, 243, 0.f) * XMMatrixTranslation(0.0f, 0.0f, 0.f)));

	_matrix MatOrtho = XMMatrixTranspose(m_pTarget_Manager->Get_OrthoMatrix(TEXT("Target_Luminance1")));

	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_SRV(TEXT("Target_HDR"), m_pShader[SHADER_LUMINANCE], "g_HDRTexture")))
		return E_FAIL;

	_float sU = (1.0f / 1280.f);
	_float sV = (1.0f / 720.f);

	_float4 Offset1 = _float4(-0.5f * sU, 0.5f * sV, 0.0f, 0.0f);
	_float4 Offset2= _float4(0.5f * sU, 0.5f * sV, 0.0f, 0.0f);
	_float4 Offset3 = _float4(-0.5f * sU, -0.5f * sV, 0.0f, 0.0f);
	_float4 Offset4 = _float4(0.5f * sU, -0.5f * sV, 0.0f, 0.0f);

	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("tcLumOffsets1", &Offset1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("tcLumOffsets2", &Offset2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("tcLumOffsets3", &Offset3, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader[SHADER_LUMINANCE]->Set_RawValue("tcLumOffsets4", &Offset4, sizeof(_float4))))
		return E_FAIL;

	m_pShader[SHADER_LUMINANCE]->Begin(0);
	
#ifdef _DEBUG
	m_pVIBuffer->Render();
#endif

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing()
{
	m_pPostFX->PostProcessing(m_pTarget_Manager->Get_SRV(L"Target_Original"));
	return S_OK;
}

void CRenderer::ResetSRV()
{
	ID3D11ShaderResourceView*		pSRVs[8] =
	{
		nullptr
	};

	m_pContext->PSSetShaderResources(0, 8, pSRVs);
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShader[SHADER_DEFERRED]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"), m_pVIBuffer, m_pShader[SHADER_DEFERRED]);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"), m_pVIBuffer, m_pShader[SHADER_DEFERRED]);

	m_pTarget_Manager->Render_Debug(TEXT("MRT_SSAO"), m_pVIBuffer, m_pShader[SHADER_SSAO]);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurX"), m_pVIBuffer, m_pShader[SHADER_BLURX]);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_BlurY"), m_pVIBuffer, m_pShader[SHADER_BLURY]);

	m_pTarget_Manager->Render_Debug(TEXT("MRT_HDR"), m_pVIBuffer, m_pShader[SHADER_HDR]);
	m_pTarget_Manager->Render_Debug(TEXT("MRT_Original"), m_pVIBuffer, m_pShader[SHADER_DEFERRED]);
	for (auto& pDebugCom : m_DebugObject)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}

	m_DebugObject.clear();

	return S_OK;
}
#endif // _DEBUG

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*			pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& List : m_RenderObjects)
	{
		for (auto& pGameObject : List)
		{
			Safe_Release(pGameObject);
		}
		List.clear();
	}

	for (_uint i = 0; i < SHADER_END; ++i)
	{
		Safe_Release(m_pShader[i]);
	}
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pPostFX);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
}
