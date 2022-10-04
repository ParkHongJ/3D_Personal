#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"
#include <algorithm>
#pragma warning(disable:4996)
CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)	
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMeshContainer::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix, vector<Mesh>* pMesh)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	Mesh mesh;
	ZeroMemory(&mesh, sizeof(Mesh));
	strcpy_s(mesh.mName, pAIMesh->mName.data);
	//strcpy_s(pMesh->mName, pAIMesh->mName.data);
	//pMesh->mMaterialIndex = pAIMesh->mMaterialIndex;
	mesh.mMaterialIndex = pAIMesh->mMaterialIndex;
#pragma region VERTEXBUFFER

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eModelType)
		hr = Ready_Vertices(pAIMesh, PivotMatrix, &mesh);
	else 
		hr = Ready_AnimVertices(pAIMesh, pModel, &mesh);

	if (FAILED(hr))
		return E_FAIL;
	
#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pAIMesh->mNumFaces;

	//pMesh->mNumFaces = pAIMesh->mNumFaces;
	mesh.mNumFaces = pAIMesh->mNumFaces;
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

	mesh.mFaces.reserve(m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
		mesh.mFaces.push_back(pIndices[i]);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	pMesh->push_back(mesh);
#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::SetUp_HierarchyNodes(CModel * pModel, aiMesh* pAIMesh, Mesh* pMesh)
{
	//m_iNumBones = pAIMesh->mNumBones;

	///* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	//for (_uint i = 0; i < m_iNumBones; ++i)
	//{
	//	aiBone*		pAIBone = pAIMesh->mBones[i];
	//	//Bone*		pBone = pMesh->mBones[i];
	//	Bone pBone;
	//	ZeroMemory(&pBone, sizeof(Bone));

	//	CHierarchyNode*		pHierarchyNode = pModel->Get_HierarchyNode(pAIBone->mName.data);

	//	_float4x4			OffsetMatrix;
	//	
	//	memcpy(&pBone->mOffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

	//	memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

	//	pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

	//	m_Bones.push_back(pHierarchyNode);

	//	Safe_AddRef(pHierarchyNode);
	//}

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
void CMeshContainer::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
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

HRESULT CMeshContainer::Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix, Mesh* pMesh)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	pMesh->mNumVertices = pAIMesh->mNumVertices;
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

	pMesh->mVertices.reserve(pMesh->mNumVertices);
	/*pMesh->mVertices = new XMFLOAT3[m_iNumVertices];
	pMesh->mNormals = new XMFLOAT3[m_iNumVertices];
	*pMesh->mTextureCoords = new XMFLOAT3[m_iNumVertices];
	pMesh->mTangents = new XMFLOAT3[m_iNumVertices];*/

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		VerticesInfo tVertices;
		ZeroMemory(&tVertices, sizeof(VerticesInfo));
		memcpy(&tVertices.mNormals, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&tVertices.mVertices, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&tVertices.mTextureCoords, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&tVertices.mTangents, &pAIMesh->mTangents[i], sizeof(_float3));
		pMesh->mVertices.push_back(tVertices);
	}
	
	//memcpy(pVertices, &pMesh->mVertices[0], sizeof(VTXMODEL) * m_iNumVertices);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel, Mesh* pMesh)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	pMesh->mNumVertices = pAIMesh->mNumVertices;

	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);


	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		VerticesInfo tVertices;
		ZeroMemory(&tVertices, sizeof(VerticesInfo));
		memcpy(&tVertices.mNormals, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&tVertices.mVertices, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&tVertices.mTextureCoords, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&tVertices.mTangents, &pAIMesh->mTangents[i], sizeof(_float3));
		pMesh->mVertices.push_back(tVertices);
	}

	pMesh->mNumBones = pAIMesh->mNumBones;
	pMesh->mBones.reserve(pMesh->mNumBones);
	//pMesh->mBones = new Bone*[pMesh->mNumBones];
	/*for (_uint i = 0; i < pMesh->mNumBones; ++i)
	{
		pMesh->mBones[i] = new Bone[pMesh->mNumBones];
		memset(pMesh->mBones[i], 0, sizeof(Bone) * pMesh->mNumBones);
	}*/
	
	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < pAIMesh->mNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];	
		Bone		pBone;
		ZeroMemory(&pBone, sizeof(Bone));

		strcpy_s(pBone.mName, pAIBone->mName.data);
		pBone.mNumWeights = pAIBone->mNumWeights;
		pBone.mWeights.reserve(pBone.mNumWeights);

		/* i번째 뼈가 어떤 정점들에게 영향ㅇ르 주는지 순회한다. */
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			_uint		iVertexIndex = pAIBone->mWeights[j].mVertexId;

			VertexWeight tVertexWeight;
			ZeroMemory(&tVertexWeight, sizeof(VertexWeight));

			tVertexWeight.mVertexId = iVertexIndex;
			tVertexWeight.mWeight = pAIBone->mWeights[j].mWeight;

			pBone.mWeights.push_back(tVertexWeight);
			/*pBone.mWeights[j].mVertexId = iVertexIndex;
			pBone.mWeights[j].mWeight = pAIBone->mWeights[j].mWeight;*/

			if(0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;	
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
		pMesh->mBones.push_back(pBone);
	}
	
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);	
	
	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix, vector<Mesh>* pMesh)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix, pMesh)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*			pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();

}
