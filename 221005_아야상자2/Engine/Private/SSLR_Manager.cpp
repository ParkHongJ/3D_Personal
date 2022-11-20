#include "..\Public\SSLR_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"
CSSLR_Manager::CSSLR_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	m_fInitDecay = 0.2f, m_fDistDecay = 0.8f, m_fMaxDeltaLen = 0.005f;
}

HRESULT CSSLR_Manager::Initialize(_uint iSizeX, _uint iSizeY)
{
	m_nWidth = iSizeX / 2;
	m_nHeight = iSizeY / 2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the occlusion resources
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_nWidth, //UINT Width;
		m_nHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	if (FAILED(m_pDevice->CreateTexture2D(&t2dDesc, NULL, &m_pOcclusionTex)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_R8_UNORM;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOcclusionTex, &UAVDesc, &m_pOcclusionUAV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd =
	{
		DXGI_FORMAT_R8_UNORM,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	dsrvd.Texture2D.MipLevels = 1;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pOcclusionTex, &dsrvd, &m_pOcclusionSRV)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the light rays resources
	t2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	if (FAILED(m_pDevice->CreateTexture2D(&t2dDesc, NULL, &m_pLightRaysTex)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd =
	{
		DXGI_FORMAT_R8_UNORM,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pLightRaysTex, &rtsvd, &m_pLightRaysRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pLightRaysTex, &dsrvd, &m_pLightRaysSRV)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the occlussion constant buffer
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory(&CBDesc, sizeof(CBDesc));
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.ByteWidth = sizeof(CB_OCCLUSSION);
	if (FAILED(m_pDevice->CreateBuffer(&CBDesc, NULL, &m_pOcclusionCB)))
		return E_FAIL;

	CBDesc.ByteWidth = sizeof(CB_LIGHT_RAYS);
	if (FAILED(m_pDevice->CreateBuffer(&CBDesc, NULL, &m_pRayTraceCB)))
		return E_FAIL;

	// Create the additive blend state
	D3D11_BLEND_DESC descBlend;
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;
	
	const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		TRUE,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL,
	};
	
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		descBlend.RenderTarget[i] = defaultRenderTargetBlendDesc;
	
	if (FAILED(m_pDevice->CreateBlendState(&descBlend, &m_pAdditiveBlendState)))
		return E_FAIL;
	
	Ready_Shaders();
	
	return S_OK;
}

HRESULT CSSLR_Manager::Ready_Shaders()
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compile the shaders
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if defined( DEBUG ) || defined( _DEBUG )
													   // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
													   // Setting this flag improves the shader debugging experience, but still allows 
													   // the shaders to be optimized and to run exactly the way they will run in 
													   // the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pShaderBlob = NULL;

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSLR.hlsl", nullptr, nullptr, "Occlussion", "cs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pOcclusionCS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);
	
	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSLR.hlsl", nullptr, nullptr, "RayTraceVS", "vs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pFullScreenVS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSLR.hlsl", nullptr, nullptr, "RayTracePS", "ps_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pRayTracePS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSLR.hlsl", nullptr, nullptr, "CombinePS", "ps_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pCombinePS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);
	return S_OK;
}

void CSSLR_Manager::PrepareOcclusion(ID3D11ShaderResourceView * pMiniDepthSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pOcclusionCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_OCCLUSSION* pOcclussion = (CB_OCCLUSSION*)MappedResource.pData;
	pOcclussion->nWidth = m_nWidth;
	pOcclussion->nHeight = m_nHeight;
	m_pContext->Unmap(m_pOcclusionCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pOcclusionCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pOcclusionUAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { pMiniDepthSRV };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pOcclusionCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CSSLR_Manager::RayTrace(const _float2 & vSunPosSS, const _float3 & vSunColor)
{
	ID3D11RenderTargetView*	pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView*	pOldDepthStencil = nullptr;

	//백버퍼의 내용을 가져와서 보관함.
	_uint		iNumViews = 8;
	m_pContext->OMGetRenderTargets(iNumViews, pOldRenderTargets, &pOldDepthStencil);

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pContext->ClearRenderTargetView(m_pLightRaysRTV, ClearColor);

	D3D11_VIEWPORT oldvp;
	UINT num = 1;
	m_pContext->RSGetViewports(&num, &oldvp);
	if (true/*!g_bShowRayTraceRes*/)
	{
		D3D11_VIEWPORT vp[1] = { { 0, 0, (_float)m_nWidth, (_float)m_nHeight, 0.0f, 1.0f } };
		m_pContext->RSSetViewports(1, vp);

		m_pContext->OMSetRenderTargets(1, &m_pLightRaysRTV, NULL);
	}

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pRayTraceCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_LIGHT_RAYS* pRayTrace = (CB_LIGHT_RAYS*)MappedResource.pData;
	pRayTrace->vSunPos = _float2(0.5f * vSunPosSS.x + 0.5f, -0.5f * vSunPosSS.y + 0.5f);
	pRayTrace->fInitDecay = m_fInitDecay;
	pRayTrace->fDistDecay = m_fDistDecay;
	pRayTrace->vRayColor = vSunColor;
	pRayTrace->fMaxDeltaLen = m_fMaxDeltaLen;
	m_pContext->Unmap(m_pRayTraceCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pRayTraceCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pOcclusionSRV };
	m_pContext->PSSetShaderResources(0, 1, arrViews);

	// Primitive settings
	m_pContext->IASetInputLayout(NULL);
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenVS, NULL, 0);
	m_pContext->GSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(m_pRayTracePS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	arrViews[0] = NULL;
	m_pContext->PSSetShaderResources(0, 1, arrViews);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->RSSetViewports(1, &oldvp);
	
	//다시 백버퍼를 바인딩
	m_pContext->OMSetRenderTargets(1, pOldRenderTargets, NULL);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(pOldRenderTargets[i]);

	Safe_Release(pOldDepthStencil);
}

void CSSLR_Manager::Combine(ID3D11RenderTargetView * pLightAccumRTV)
{
	ID3D11BlendState* pPrevBlendState;
	FLOAT prevBlendFactor[4];
	UINT prevSampleMask;
	m_pContext->OMGetBlendState(&pPrevBlendState, prevBlendFactor, &prevSampleMask);
	m_pContext->OMSetBlendState(m_pAdditiveBlendState, prevBlendFactor, prevSampleMask);

	// Restore the light accumulation view
	m_pContext->OMSetRenderTargets(1, &pLightAccumRTV, NULL);

	// Constants
	ID3D11Buffer* arrConstBuffers[1] = { m_pRayTraceCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pLightRaysSRV };
	m_pContext->PSSetShaderResources(0, 1, arrViews);

	// Primitive settings
	m_pContext->IASetInputLayout(NULL);
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenVS, NULL, 0);
	m_pContext->GSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(m_pCombinePS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	arrViews[0] = NULL;
	m_pContext->PSSetShaderResources(0, 1, arrViews);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->OMSetBlendState(pPrevBlendState, prevBlendFactor, prevSampleMask);
}

void CSSLR_Manager::Render(ID3D11RenderTargetView * pLightAccumRTV, ID3D11ShaderResourceView * pMiniDepthSRV, const _float3 & vSunDir, const _float3 & vSunColor)
{
	// No need to do anything if the camera is facing away from the sun
	// This will not work if the FOV is close to 180 or higher
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	//카메라 룩

	//memcpy(&m_vCamPosition, &m_TransformInverseMatrix[D3DTS_VIEW].m[3][0], sizeof(_float4));
	_float4x4 matViewInv;
	XMStoreFloat4x4(&matViewInv, pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW));
	_float4 vCamLook;
	memcpy(&vCamLook, &matViewInv.m[3][0], sizeof(_float4));
	const float dotCamSun = - XMVectorGetX(XMVector3Dot(XMLoadFloat4(&vCamLook), XMLoadFloat3(&vSunDir)));
	if (dotCamSun <= 0.0f)
	{
		return;
	}

	_float3 vSunPos;
	XMStoreFloat3(&vSunPos, -200.0f * XMLoadFloat3(&vSunDir));

	
	_float4 vEyePos = pPipeLine->Get_CamPosition();//*g_Camera.GetEyePt();
	vSunPos.x += vEyePos.x;
	vSunPos.z += vEyePos.z;
	_float4x4 mView = pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);//*g_Camera.GetViewMatrix();
	_float4x4 mProj = pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);//*g_Camera.GetProjMatrix();
	_float4x4 mViewProjection;
	XMStoreFloat4x4(&mViewProjection, XMLoadFloat4x4(&mView) *XMLoadFloat4x4(&mProj));


	_float3 vSunPosSS;
	XMStoreFloat3(&vSunPosSS, XMVector3TransformCoord(XMLoadFloat3(&vSunPos), XMLoadFloat4x4(&mViewProjection)));

	// If the sun is too far out of view we just want to turn off the effect
	static const float fMaxSunDist = 1.3f;
	if (fabs(vSunPosSS.x) >= fMaxSunDist || fabs(vSunPosSS.y) >= fMaxSunDist)
	{
		return;
	}

	// Attenuate the sun color based on how far the sun is from the view
	_float3 vSunColorAtt = vSunColor;
	float fMaxDist = max(fabs(vSunPosSS.x), fabs(vSunPosSS.y));
	if (fMaxDist >= 1.0f)
	{
		XMStoreFloat3(&vSunColorAtt, XMLoadFloat3(&vSunColorAtt) * (fMaxSunDist - fMaxDist));
	}

	RELEASE_INSTANCE(CPipeLine);

	PrepareOcclusion(pMiniDepthSRV);
	RayTrace(_float2(vSunPosSS.x, vSunPosSS.y), vSunColorAtt);

	Combine(pLightAccumRTV);
}

CSSLR_Manager * CSSLR_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY)
{
	CSSLR_Manager*		pInstance = new CSSLR_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY)))
	{
		MSG_BOX(TEXT("Failed To Created : CSSLR_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSSLR_Manager::Free()
{
	Safe_Release(m_pOcclusionTex);
	Safe_Release(m_pOcclusionUAV);
	Safe_Release(m_pOcclusionSRV);
	Safe_Release(m_pLightRaysTex);
	Safe_Release(m_pLightRaysRTV);
	Safe_Release(m_pLightRaysSRV);
	Safe_Release(m_pOcclusionCB);
	Safe_Release(m_pOcclusionCS);
	Safe_Release(m_pRayTraceCB);
	Safe_Release(m_pFullScreenVS);
	Safe_Release(m_pRayTracePS);
	Safe_Release(m_pCombinePS);
	Safe_Release(m_pAdditiveBlendState);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
