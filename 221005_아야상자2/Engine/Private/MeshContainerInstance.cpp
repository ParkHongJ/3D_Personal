#include "..\Public\MeshContainerInstance.h"
#include "Model.h"
#include "HierarchyNode.h"

CMeshContainerInstance::CMeshContainerInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMeshContainerInstance::CMeshContainerInstance(const CMeshContainerInstance & rhs)
	: CVIBuffer(rhs)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iNumIndices(rhs.m_iNumIndices)
{
	strcpy_s(m_szName, rhs.m_szName);
	Safe_AddRef(m_pVBInstance);
}

HRESULT CMeshContainerInstance::Initialize_Prototype(_uint iNumInstance, Mesh * pMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	m_iNumInstance = iNumInstance;
	/* 이 메시와 이름이 같은 뼈대가 존재한다면.
	이 뼈대의 행렬을 메시를 구성하는 정점에 곱해질 수 있도록 유도하낟. */
	
	strcpy_s(m_szName, pMesh->mName);
	/* 메시마다 사용하는 머테리얼(텍스쳐정보로 표현)이 다른다. */
	/* 메시를 그릴때 마다 어떤 머테리얼을 솅디ㅓ로 던져야할 지르르 결정해주기위해서. */
	m_iMaterialIndex = pMesh->mMaterialIndex;
	m_iNumIndices = pMesh->mNumFaces;
#pragma region VERTEXBUFFER
	if (FAILED(Ready_Vertices(pMesh, PivotMatrix)))
		return E_FAIL;
#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pMesh->mNumFaces * iNumInstance;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);
	
	for (_uint i = 0; i < iNumInstance; ++i)
	{
		memcpy(pIndices, pMesh->mFaces.data(), sizeof(FACEINDICES32) * pMesh->mFaces.size());
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


#pragma region INSTANCEBUFFER
	m_iInstanceStride = sizeof(VTXMODELINSANCE);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXMODELINSANCE*		pInstanceVtx = new VTXMODELINSANCE[iNumInstance];
	ZeroMemory(pInstanceVtx, sizeof(VTXMODELINSANCE) * iNumInstance);

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVtx[i].vRight = _float4(0.01f, 0.f, 0.f, 0.f);
		pInstanceVtx[i].vUp = _float4(0.f, 0.01f, 0.f, 0.f);
		pInstanceVtx[i].vLook = _float4(0.f, 0.f, 0.01f, 0.f);
		pInstanceVtx[i].vPosition = _float4((_float)(rand() % 31 - 30), 0.f, (_float)(rand() % 31 - 30), 1.f);
	}
	//rand() % (b - a + 1) + a == > a부터 b의 범위이다.
	//rand() % (100 - (-100) + 1 ) -100
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);

#pragma endregion
	return S_OK;
}

HRESULT CMeshContainerInstance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainerInstance::SetUp_HierarchyNodes(CModel * pModel, Mesh* pMesh)
{
	m_iNumBones = pMesh->mNumBones;



	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		Bone*		pBone = &pMesh->mBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->Get_HierarchyNode(pBone->mName);

		_float4x4			OffsetMatrix;

		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));

		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.push_back(pHierarchyNode);

		Safe_AddRef(pHierarchyNode);
	}

	if (0 == m_iNumBones)
	{

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

		if (nullptr == pNode)
			return S_OK;

		m_iNumBones = 1;

		m_Bones.push_back(pNode);

	}

	return S_OK;
}

/* 메시의 정점을 그리기위해 셰이더에 넘기기위한 뼈행렬의 배열을 구성한다. */
void CMeshContainerInstance::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&pBoneMatrices[i], XMMatrixTranspose(m_Bones[i]->Get_OffSetMatrix() * m_Bones[i]->Get_CombinedTransformation() * PivotMatrix));
	}

}

HRESULT CMeshContainerInstance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint			iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint			iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iNumPrimitives * m_iNumIndicesofPrimitive, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CMeshContainerInstance::Ready_Vertices(Mesh* pMesh, _fmatrix PivotMatrix)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = pMesh->mNumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	memcpy(pVertices, pMesh->mVertices.data(), sizeof(VTXMODEL) * m_iNumVertices);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMeshContainerInstance * CMeshContainerInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance, Mesh * pMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	CMeshContainerInstance*			pInstance = new CMeshContainerInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance, pMesh, pModel, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainerInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainerInstance::Clone(void * pArg)
{
	CMeshContainerInstance*			pInstance = new CMeshContainerInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMeshContainerInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainerInstance::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();

	Safe_Release(m_pVBInstance);
}
