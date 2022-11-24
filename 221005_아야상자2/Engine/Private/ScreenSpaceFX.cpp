#include "..\Public\ScreenSpaceFX.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"
CScreenSpaceFX::CScreenSpaceFX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CScreenSpaceFX::Initialize(_uint iSizeX, _uint iSizeY)
{
	m_nWidth = iSizeX / 2;
	m_nHeight = iSizeY / 2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate SSAO
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_nWidth, //UINT Width;
		m_nHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R32_TYPELESS,//DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};
	if (FAILED(m_pDevice->CreateTexture2D(&t2dDesc, NULL, &m_pSSAO_RT)))
		return E_FAIL;

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pSSAO_RT, &UAVDesc, &m_pSSAO_UAV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSSAO_RT, &SRVDesc, &m_pSSAO_SRV)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled depth buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = 4 * sizeof(float);
	bufferDesc.ByteWidth = m_nWidth * m_nHeight * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pMiniDepthBuffer)))
		return E_FAIL;

	ZeroMemory(&UAVDesc, sizeof(UAVDesc));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pMiniDepthBuffer, &UAVDesc, &m_pMiniDepthUAV)))
		return E_FAIL;

	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pMiniDepthBuffer, &SRVDesc, &m_pMiniDepthSRV)))
		return E_FAIL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scale depth constant buffer
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory(&CBDesc, sizeof(CBDesc));
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.ByteWidth = sizeof(TDownscaleCB);
	if (FAILED(m_pDevice->CreateBuffer(&CBDesc, NULL, &m_pDownscaleCB)))
		return E_FAIL;

	Ready_Shaders();

	return S_OK;
}

HRESULT CScreenSpaceFX::Ready_Shaders()
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
	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSAO.hlsl", nullptr, nullptr, "DepthDownscale", "cs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDepthDownscaleCS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/SSAO.hlsl", nullptr, nullptr, "SSAOCompute", "cs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pComputeCS)))
		return E_FAIL;

	Safe_Release(pShaderBlob);
	return S_OK;
}

void CScreenSpaceFX::Compute(ID3D11ShaderResourceView * pDepthSRV, ID3D11ShaderResourceView * pNormalsSRV)
{
	ID3D11RenderTargetView*	pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView*	pOldDepthStencil = nullptr;

	//백버퍼의 내용을 가져와서 보관함.
	_uint		iNumViews = 8;
	m_pContext->OMGetRenderTargets(iNumViews, pOldRenderTargets, &pOldDepthStencil);

	ID3D11ShaderResourceView *arrSRV[1] = { NULL };
	m_pContext->PSSetShaderResources(5, 1, arrSRV);

	DownscaleDepth(pDepthSRV, pNormalsSRV);
	ComputeSSAO();
	Blur();

	arrSRV[0] = m_pSSAO_SRV;

	//다시 백버퍼를 바인딩
	m_pContext->OMSetRenderTargets(1, pOldRenderTargets, pOldDepthStencil);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(pOldRenderTargets[i]);

	Safe_Release(pOldDepthStencil);
}

void CScreenSpaceFX::DownscaleDepth(ID3D11ShaderResourceView * pDepthSRV, ID3D11ShaderResourceView * pNormalsSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pDownscaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TDownscaleCB* pDownscale = (TDownscaleCB*)MappedResource.pData;
	pDownscale->nWidth = m_nWidth;
	pDownscale->nHeight = m_nHeight;
	pDownscale->fHorResRcp = 1.0f / (float)pDownscale->nWidth;
	pDownscale->fVerResRcp = 1.0f / (float)pDownscale->nHeight;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_float4x4 pProj;
	XMStoreFloat4x4(&pProj, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	
	//const D3DXMATRIX* pProj = g_Camera.GetProjMatrix();
	pDownscale->ProjParams.x = 1.0f / pProj.m[0][0];
	pDownscale->ProjParams.y = 1.0f / pProj.m[1][1];
	//0.2, 300

	// Near / (Far - Near)
	_float fNear = 0.2f;
	_float fFar = 300.f;
	float fQ = fNear / (fFar - fNear);
	pDownscale->ProjParams.z = -fFar * fQ;
	pDownscale->ProjParams.w = -fQ;
	pDownscale->ViewMatrix = pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	//D3DXMatrixTranspose(&pDownscale->ViewMatrix, g_Camera.GetViewMatrix());
	pDownscale->fOffsetRadius = (float)m_iSSAOSampRadius;
	pDownscale->fRadius = m_fRadius;
	pDownscale->fMaxDepth = fFar;

	m_pContext->Unmap(m_pDownscaleCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pMiniDepthUAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pDepthSRV, pNormalsSRV };
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pDepthDownscaleCS, NULL, 0);

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
	
	RELEASE_INSTANCE(CPipeLine);
}

void CScreenSpaceFX::ComputeSSAO()
{
	// Constants
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pSSAO_UAV };
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pMiniDepthSRV };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader(m_pComputeCS, NULL, 0);

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

void CScreenSpaceFX::Blur()
{
}

CScreenSpaceFX * CScreenSpaceFX::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY)
{
	CScreenSpaceFX*		pInstance = new CScreenSpaceFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY)))
	{
		MSG_BOX(TEXT("Failed To Created : CScreenSpaceFX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScreenSpaceFX::Free()
{
	Safe_Release(m_pDownscaleCB);
	Safe_Release(m_pSSAO_RT);
	Safe_Release(m_pSSAO_SRV);
	Safe_Release(m_pSSAO_UAV);
	Safe_Release(m_pMiniDepthBuffer);
	Safe_Release(m_pMiniDepthUAV);
	Safe_Release(m_pMiniDepthSRV);
	Safe_Release(m_pDepthDownscaleCS);
	Safe_Release(m_pComputeCS);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
