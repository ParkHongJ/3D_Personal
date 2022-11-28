#include "..\Public\VIBuffer_Point.h"

CVIBuffer_Point::CVIBuffer_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point & rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
#pragma region VERTEXBUFFER
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOINT);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	//VTXTEX*		pVertices = new VTXTEX[4];
	//ZeroMemory(pVertices, sizeof(VTXTEX) * 4);

	//pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	//pVertices[0].vTexture = _float2(0.f, 0.f);

	//pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	//pVertices[1].vTexture = _float2(1.f, 0.f);

	//pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	//pVertices[2].vTexture = _float2(1.f, 1.f);

	//pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//pVertices[3].vTexture = _float2(0.f, 1.f);

	VTXPOINT*		pVertices = new VTXPOINT;

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices->vSize = _float2(1.5f, 1.5f);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = 1;
	m_iIndexSizeofPrimitive = sizeof(_ushort);
	m_iNumIndicesofPrimitive = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	//FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitives];
	//ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);

	//pIndices[0]._0 = 0;
	//pIndices[0]._1 = 1;
	//pIndices[0]._2 = 2;

	//pIndices[1]._0 = 0;
	//pIndices[1]._1 = 2;
	//pIndices[1]._2 = 3;

	_ushort*		pIndices = new _ushort[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitives);
	/*FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitives);*/

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Point::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Point::SetSize(_float fX, _float fY)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);
	//TDownscaleCB* pDownscale = (TDownscaleCB*)MappedResource.pData;
	((VTXPOINT*)MappedSubResource.pData)->vSize = _float2(fX, fY);
	/*for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y += m_pInstanceSpeeds[i] * fTimeDelta;
		if (3.0f <= ((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y)
			((VTXINSTANCE*)MappedSubResource.pData)[i].vPosition.y = 0.f;
	}*/

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Point * CVIBuffer_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Point*			pInstance = new CVIBuffer_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point::Clone(void * pArg)
{
	CVIBuffer_Point*			pInstance = new CVIBuffer_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point::Free()
{
	__super::Free();
}
