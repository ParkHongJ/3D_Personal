#include "..\Public\PostFX.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"


//HRESULT CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile, DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer);

CPostFX::CPostFX(ID3D11Device * pDevice, ID3D11DeviceContext * pContext) : m_fMiddleGrey(0.5f), m_fWhite(5.7f),
m_pDownScale1DBuffer(NULL), m_pDownScale1DUAV(NULL), m_pDownScale1DSRV(NULL),
m_pDownScaleCB(NULL), m_pFinalPassCB(NULL),
m_pAvgLumBuffer(NULL), m_pAvgLumUAV(NULL), m_pAvgLumSRV(NULL),
m_pDownScaleFirstPassCS(NULL), m_pDownScaleSecondPassCS(NULL), m_pFullScreenQuadVS(NULL), m_pFinalPassPS(NULL)
, m_pDevice(pDevice)
, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPostFX::Initialize(_uint iSizeX, _uint iSizeY)
{
	//샘플링 옵션설정
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	//샘플러 생성
	//이걸로 HDR텍스쳐 샘플링할거임
	if (FAILED(m_pDevice->CreateSamplerState(&samDesc, &g_pSampPoint)))
		return E_FAIL;

	//톤매핑은 두개의 버퍼 
	//두개의 상수버퍼 
	//두개의 컴퓨트 셰이더를 이용해 씬의 휘도를 계산한다

	// Find the amount of thread groups needed for the downscale operation
	m_nWidth = iSizeX;
	m_nHeight = iSizeY;
	m_nDownScaleGroups = (UINT)ceil((float)(m_nWidth * m_nHeight / 16) / 1024.0f);


	//첫번째 버퍼는 휘도계산시 중간값 저장함.
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled luminance buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	//D3D11_BIND_UNORDERED_ACCESS : Pixel셰이더나 컴퓨트셰이더에서 병렬적으로 실행하는 버퍼
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(float);
	bufferDesc.ByteWidth = m_nDownScaleGroups * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScale1DBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER; //컴퓨트셰이더상에서 이 버퍼를 일반적인 버퍼로 보겠다. (샘플링을 하지않음, 어떠한 수정도 없다)
	DescUAV.Buffer.NumElements = m_nDownScaleGroups;
	if (FAILED((m_pDevice->CreateUnorderedAccessView(m_pDownScale1DBuffer, &DescUAV, &m_pDownScale1DUAV))))
		return E_FAIL;


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate average luminance buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
	ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	dsrvd.Format = DXGI_FORMAT_UNKNOWN;
	dsrvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	dsrvd.Buffer.NumElements = m_nDownScaleGroups;
	if (FAILED((m_pDevice->CreateShaderResourceView(m_pDownScale1DBuffer, &dsrvd, &m_pDownScale1DSRV))))
		return E_FAIL;
	
	bufferDesc.ByteWidth = sizeof(float);
	if (FAILED((m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pAvgLumBuffer))))
		return E_FAIL;
	
	DescUAV.Buffer.NumElements = 1;
	if (FAILED((m_pDevice->CreateUnorderedAccessView(m_pAvgLumBuffer, &DescUAV, &m_pAvgLumUAV))))
		return E_FAIL;
	
	dsrvd.Buffer.NumElements = 1;
	if (FAILED((m_pDevice->CreateShaderResourceView(m_pAvgLumBuffer, &dsrvd, &m_pAvgLumSRV))))
		return E_FAIL;
	
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 이 버퍼는 CPU가 액세스 할 수 있다.
	bufferDesc.ByteWidth = sizeof(TDownScaleCB);
	if (FAILED((m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDownScaleCB))))
		return E_FAIL;
	bufferDesc.ByteWidth = sizeof(TFinalPassCB);
	if (FAILED((m_pDevice->CreateBuffer(&bufferDesc, NULL, &m_pFinalPassCB))))
		return E_FAIL;

	// Compile the shaders
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	//WCHAR str[MAX_PATH];
	ID3DBlob* pShaderBlob = NULL;
	/*if (FAILED(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"PostDownScaleFX.hlsl")))
		return E_FAIL;
    V_RETURN( CompileShader(str, NULL, "DownScaleFirstPass", "cs_5_0", dwShaderFlags, &pShaderBlob) );*/

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/PostDownScaleFX.hlsl", nullptr, nullptr, "DownScaleFirstPass", "cs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDownScaleFirstPassCS)))
		return E_FAIL;
	Safe_Release( pShaderBlob );

	//V_RETURN( CompileShader(str, NULL, "DownScaleSecondPass", "cs_5_0", dwShaderFlags, &pShaderBlob) );

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/PostDownScaleFX.hlsl", nullptr, nullptr, "DownScaleSecondPass", "cs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pDownScaleSecondPassCS)))
		return E_FAIL;

	Safe_Release( pShaderBlob );

	//V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"PostFX.hlsl" ) );
    //V_RETURN( CompileShader(str, NULL, "FullScreenQuadVS", "vs_5_0", dwShaderFlags, &pShaderBlob) );

	if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/PostFX.hlsl", nullptr, nullptr, "FullScreenQuadVS", "vs_5_0", 0, 0, &pShaderBlob, nullptr)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pFullScreenQuadVS)))
		return E_FAIL;

   Safe_Release( pShaderBlob );

   /*if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/PostFX.hlsl", NULL, "FinalPassPS", "ps_5_0", dwShaderFlags, &pShaderBlob)))
	   return E_FAIL;*/
   if (FAILED(D3DCompileFromFile(L"../Bin/ShaderFiles/PostFX.hlsl", nullptr, nullptr, "FinalPassPS", "ps_5_0", 0, 0, &pShaderBlob, nullptr)))
	   return E_FAIL;
	if (FAILED(m_pDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pFinalPassPS)))
		return E_FAIL; 

	Safe_Release(pShaderBlob);

	return S_OK;
}

void CPostFX::Deinit()
{
	Safe_Release( g_pSampPoint );
	Safe_Release( m_pDownScale1DBuffer );
	Safe_Release( m_pDownScale1DUAV );
	Safe_Release( m_pDownScale1DSRV );
	Safe_Release( m_pDownScaleCB );
	Safe_Release( m_pFinalPassCB );
	Safe_Release( m_pAvgLumBuffer );
	Safe_Release( m_pAvgLumUAV );
	Safe_Release( m_pAvgLumSRV );
	Safe_Release( m_pDownScaleFirstPassCS );
	Safe_Release( m_pDownScaleSecondPassCS );
	Safe_Release( m_pFullScreenQuadVS );
	Safe_Release( m_pFinalPassPS );
}

void CPostFX::PostProcessing(ID3D11ShaderResourceView* pHDRSRV)
{
	ID3D11RenderTargetView*	pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView*	pOldDepthStencil = nullptr;

	_uint		iNumViews = 8;
	//백버퍼의 내용을 가져와서 보관함.
	m_pContext->OMGetRenderTargets(iNumViews, pOldRenderTargets, &pOldDepthStencil);

	// Down scale the HDR image
	ID3D11RenderTargetView* rt[1] = { NULL };
	m_pContext->OMSetRenderTargets( 1, rt, NULL );
	DownScale(pHDRSRV);

	// Do the final pass
	rt[0] = pOldRenderTargets[0];
	m_pContext->OMSetRenderTargets( 1, rt, NULL );
	FinalPass(pHDRSRV);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(pOldRenderTargets[i]);

	Safe_Release(pOldDepthStencil);
}

void CPostFX::DownScale(ID3D11ShaderResourceView* pHDRSRV)
{
	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pDownScale1DUAV };
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, (UINT*)(&arrUAVs) ); //뷰에 바인딩

	// Input
	ID3D11ShaderResourceView* arrViews[1] = {pHDRSRV};
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pDownScaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TDownScaleCB* pDownScale = (TDownScaleCB*)MappedResource.pData; 
	pDownScale->nWidth = m_nWidth  / 4;
	pDownScale->nHeight = m_nHeight / 4;
	pDownScale->nTotalPixels = pDownScale->nWidth * pDownScale->nHeight;
	pDownScale->nGroupSize = m_nDownScaleGroups;
	m_pContext->Unmap(m_pDownScaleCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);


	// Shader
	//파이프라인 스테이지에 바인딩
	m_pContext->CSSetShader( m_pDownScaleFirstPassCS, NULL, 0 );

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer

	//스레드 그룹생성 m_nDownScaleGroups * 1 * 1 개
	//GPU에게 현재 바인딩된 내용을 바탕으로 연산해줄 것을 요청
	m_pContext->Dispatch(m_nDownScaleGroups, 1, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - reduce to a single pixel

	// Outoput
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	arrUAVs[0] = m_pAvgLumUAV;
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, (UINT*)(&arrUAVs) );

	// Input
	arrViews[0] = m_pDownScale1DSRV;
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Constants
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Shader
	m_pContext->CSSetShader( m_pDownScaleSecondPassCS, NULL, 0 );

	// Excute with a single group - this group has enough threads to process all the pixels
	m_pContext->Dispatch(1, 1, 1);

	// Cleanup
	m_pContext->CSSetShader( NULL, NULL, 0 );
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, (UINT*)(&arrUAVs) );
}

void CPostFX::FinalPass(ID3D11ShaderResourceView* pHDRSRV)
{
	ID3D11ShaderResourceView* arrViews[2] = {pHDRSRV, m_pAvgLumSRV};
	m_pContext->PSSetShaderResources(0, 2, arrViews);

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pFinalPassCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TFinalPassCB* pFinalPass = (TFinalPassCB*)MappedResource.pData; 
	pFinalPass->fMiddleGrey = m_fMiddleGrey;
	pFinalPass->fLumWhiteSqr = m_fWhite;
	pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
	pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Squre
	m_pContext->Unmap(m_pFinalPassCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pFinalPassCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	m_pContext->IASetInputLayout( NULL );
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
	m_pContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	ID3D11SamplerState* arrSamplers[1] = { g_pSampPoint };
	m_pContext->PSSetSamplers( 0, 1, arrSamplers );

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenQuadVS, NULL, 0);
	m_pContext->PSSetShader(m_pFinalPassPS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->PSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
}

CPostFX * CPostFX::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
{
	CPostFX*		pInstance = new CPostFX(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY)))
	{
		MSG_BOX(TEXT("Failed To Created : CPostFX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPostFX::Free()
{
	Deinit();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

//HRESULT CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO * pMacros, char * strEntryPoint, char * strProfile, DWORD dwShaderFlags, ID3DBlob ** ppVertexShaderBuffer)
//{
//	HRESULT hr;
//	ID3DBlob* pErrorBlob;
//	if (FAILED(hr = D3DX11CompileFromFile(strPath, pMacros, NULL, strEntryPoint, strProfile, dwShaderFlags, 0, NULL,
//		ppVertexShaderBuffer, &pErrorBlob, NULL)))
//	{
//		int buffSize = pErrorBlob->GetBufferSize() + 1;
//		LPWSTR gah = new wchar_t[buffSize];
//		MultiByteToWideChar(CP_ACP, 0, (char*)pErrorBlob->GetBufferPointer(), buffSize, gah, buffSize);
//		OutputDebugString(gah);
//		delete gah;
//		OutputDebugString(L"\n");
//	}
//	return hr;
//}
