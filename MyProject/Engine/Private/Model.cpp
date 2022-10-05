#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"
#include <chrono>

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	//, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	/*, m_HierarchyNodes(rhs.m_HierarchyNodes)*/
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_TempScene(rhs.m_TempScene)
	, m_bClone(true)
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);
	

}

CHierarchyNode * CModel::Get_HierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode) 
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;	
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

void CModel::Change_Animation(_uint iAnimIndex)
{
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		_uint iPrevAnimIndex = m_iCurrentAnimIndex;
		m_iCurrentAnimIndex = iAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->Change_Animation(m_Animations[iPrevAnimIndex]);
		m_Animations[iPrevAnimIndex]->ResetKeyFrames();
	}
}

_uint CModel::Get_AnimBoneSize(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_BoneSize();
}

_uint CModel::GetAnimSize()
{
	return m_Animations.size();
}

HRESULT CModel::LoadBinary(const _tchar* ModelFilePath)
{
	HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	m_TempScene = new TEMPSCENE;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	ReadFile(hFile, &m_TempScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);

	//m_TempScene->mRootNode = new Node;

	LoadNode(hFile, &m_TempScene->mRootNode, dwByte, dwStrByte);

	//m_TempScene->mAnimations = new Animation*[m_TempScene->mNumAnimations];
	//m_TempScene->mMesh = new Mesh[m_TempScene->mNumMeshes];

	//Mesh로드
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		Mesh pMesh;/* = &m_TempScene->mMesh[i];*/
		ZeroMemory(&pMesh, sizeof(Mesh));
		//Mesh이름 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		/*char*	pName = nullptr;
		pName = new char[dwStrByte + 1];*/
		ReadFile(hFile, pMesh.mName, dwStrByte, &dwByte, nullptr);
		//pName[dwByte / sizeof(char)] = 0;
		//strcpy_s(pMesh.mName, pName);

		//Safe_Delete_Array(pName);

		ReadFile(hFile, &pMesh.mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumFaces, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumBones, sizeof(_uint), &dwByte, nullptr);

		/*for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
		WriteFile(hFile, &pMesh->mVertices[j], sizeof(VerticesInfo), &dwByte, nullptr);
		}*/

		//Vertices로드
		pMesh.mVertices.reserve(pMesh.mNumVertices);
		for (_uint j = 0; j < pMesh.mNumVertices; ++j)
		{
			VerticesInfo vInfo;
			ZeroMemory(&vInfo, sizeof(VerticesInfo));
			ReadFile(hFile, &vInfo, sizeof(VerticesInfo), &dwByte, nullptr);
			pMesh.mVertices.push_back(vInfo);
		}
		int a = 10;
		//pMesh->mVertices = new XMFLOAT3[pMesh->mNumVertices];
		//pMesh->mNormals = new XMFLOAT3[pMesh->mNumVertices];
		//pMesh->mTangents = new XMFLOAT3[pMesh->mNumVertices];
		//*pMesh->mTextureCoords = new XMFLOAT3[pMesh->mNumVertices];
		//
		//for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		//{
		//	ReadFile(hFile, &pMesh->mVertices[j], sizeof(XMFLOAT3), &dwByte, nullptr);
		//	ReadFile(hFile, &pMesh->mNormals[j], sizeof(XMFLOAT3), &dwByte, nullptr);
		//	ReadFile(hFile, &pMesh->mTangents[j], sizeof(XMFLOAT3), &dwByte, nullptr);
		//}

		////TextureCoord로드
		//for (_uint j = 0; j < 8; ++j)
		//{
		//	ReadFile(hFile, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT3), &dwByte, nullptr);
		//}

		//	pMesh->mBones = new Bone*[pMesh->mNumBones];

		//Bone로드
		pMesh.mBones.reserve(pMesh.mNumBones);
		for (_uint j = 0; j < pMesh.mNumBones; ++j)
		{
			//pMesh->mBones[j] = new Bone;
			Bone pBone;// = pMesh->mBones[j];
			ZeroMemory(&pBone, sizeof(Bone));

			//BoneName로드
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			/*char*	pName = nullptr;
			pName = new char[dwStrByte + 1];*/
			ReadFile(hFile, pBone.mName, dwStrByte, &dwByte, nullptr);
			/*pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pBone.mName, pName);

			Safe_Delete_Array(pName);*/
			//OffsetMatrix로드
			ReadFile(hFile, &pBone.mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

			//NumWeights로드
			ReadFile(hFile, &pBone.mNumWeights, sizeof(_uint), &dwByte, nullptr);

			//pBone->mWeights = new VertexWeight[pBone->mNumWeights];
			pBone.mWeights.reserve(pBone.mNumWeights);
			for (_uint k = 0; k < pBone.mNumWeights; ++k)
			{
				VertexWeight Weight;
				ZeroMemory(&Weight, sizeof(VertexWeight));

				ReadFile(hFile, &Weight, sizeof(VertexWeight), &dwByte, nullptr);

				pBone.mWeights.push_back(Weight);
				/*ReadFile(hFile, &pBone->mWeights[k].mVertexId, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, &pBone->mWeights[k].mWeight, sizeof(_float), &dwByte, nullptr);*/
			}
			pMesh.mBones.push_back(pBone);
		}
		//	//Face로드
		pMesh.mFaces.reserve(pMesh.mNumFaces);
		for (_uint j = 0; j < pMesh.mNumFaces; ++j)
		{
			FACEINDICES32 mFace;
			ZeroMemory(&mFace, sizeof(FACEINDICES32));
			ReadFile(hFile, &mFace, sizeof(FACEINDICES32), &dwByte, nullptr);
			pMesh.mFaces.push_back(mFace);
			/*WriteFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);*/
		}
		//	//Face로드
		//	pMesh->mFaces = new Face[pMesh->mNumFaces];
		//	for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		//	{
		//		ReadFile(hFile, &pMesh->mFaces[j].mIndices[0], sizeof(_uint), &dwByte, nullptr);
		//		ReadFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
		//		ReadFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);
		//	}
		//}
		m_TempScene->mMesh.push_back(pMesh);
	}
	//Material로드
	for (_uint i = 0; i < m_TempScene->mNumMaterials; ++i)
	{
		//TextureName로드
		Material pMaterial;
		ZeroMemory(&pMaterial, sizeof(Material));
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		/*_tchar*	pName = nullptr;
		pName = new _tchar[dwStrByte];*/
		ReadFile(hFile, pMaterial.mName, dwStrByte, &dwByte, nullptr);
		/*pName[dwByte / sizeof(_tchar)] = 0;
		wcscpy_s(pMaterial.mName, pName);*/

		ReadFile(hFile, &pMaterial.TextureType, sizeof(_uint), &dwByte, nullptr);

		//Safe_Delete_Array(pName);
		m_TempScene->mMaterials.push_back(pMaterial);
	}

	//Animation로드

	//m_TempScene->mAnimations = new ANIMATION*[m_TempScene->mNumAnimations];
	m_TempScene->mAnimations.reserve(m_TempScene->mNumAnimations);
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		//m_TempScene->mAnimations[i] = new ANIMATION;
		Animation pAnimation;// = m_TempScene->mAnimations[i];
		ZeroMemory(&pAnimation, sizeof(Animation));

		//AnimationName로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		/*char*	pName = nullptr;
		pName = new char[dwStrByte + 1];*/
		ReadFile(hFile, pAnimation.mName, dwStrByte, &dwByte, nullptr);
		/*pName[dwByte / sizeof(char)] = 0;
		strcpy_s(pAnimation.mName, pName);

		Safe_Delete_Array(pName);*/
		//Animation정보로드
		ReadFile(hFile, &pAnimation.mDuration, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation.mTickPerSecond, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation.mNumChannels, sizeof(_uint), &dwByte, nullptr);

		//pAnimation->mChannels = new NodeAnim*[pAnimation->mNumChannels];
		pAnimation.mChannels.reserve(pAnimation.mNumChannels);
		for (_uint j = 0; j < pAnimation.mNumChannels; ++j)
		{
			//pAnimation->mChannels[j] = new NodeAnim;
			NodeAnim pNodeAnim;// = pAnimation->mChannels[j];
			ZeroMemory(&pNodeAnim, sizeof(NodeAnim));

			//NodeName로드
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			/*char*	pName = nullptr;
			pName = new char[dwStrByte + 1];*/
			ReadFile(hFile, pNodeAnim.mNodeName, dwStrByte, &dwByte, nullptr);
			/*pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pNodeAnim.mNodeName, pName);

			Safe_Delete_Array(pName);*/
			ReadFile(hFile, &pNodeAnim.mNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
			pNodeAnim.mKeyFrames.reserve(pNodeAnim.mNumKeyFrames);
			for (_uint k = 0; k < pNodeAnim.mNumKeyFrames; ++k)
			{
				KEYFRAME keyFrame;
				ZeroMemory(&keyFrame, sizeof(KEYFRAME));
				ReadFile(hFile, &keyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
				pNodeAnim.mKeyFrames.push_back(keyFrame);
			}
			pAnimation.mChannels.push_back(pNodeAnim);
		}
		m_TempScene->mAnimations.push_back(pAnimation);
	}

	if (0 == dwByte)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);
	//return S_OK;
	return S_OK;
}

void CModel::LoadNode(HANDLE hFile, Node * pNode, DWORD & dwByte, DWORD & dwStrByte)
{
	ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	/*char*	pName = nullptr;
	pName = new char[dwStrByte + 1];*/
	ReadFile(hFile, pNode->mName, dwStrByte, &dwByte, nullptr);
	//pName[dwByte / sizeof(char)] = 0;
	//strcpy_s(pNode->mName, pName);

	//Safe_Delete_Array(pName);
	ReadFile(hFile, &pNode->mNumChildren, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &pNode->mTransformation, sizeof(_float4x4), &dwByte, nullptr);

	pNode->mChildren.reserve(pNode->mNumChildren);
	//pNode->mChildren = new Node*[pNode->mNumChildren];
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		//pNode->mChildren[i] = new Node;
		Node tempNode;
		ZeroMemory(&tempNode, sizeof(Node));
		LoadNode(hFile, &tempNode, dwByte, dwStrByte);
		pNode->mChildren.push_back(tempNode);
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar * pModelFilePath, _fmatrix PivotMatrix)
{
	LoadBinary(pModelFilePath);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_eModelType = eType;

	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	Ready_HierarchyNodes(nullptr, 0, &m_TempScene->mRootNode);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (TYPE_ANIM == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{
			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
			{
				pMeshContainer->SetUp_HierarchyNodes(this, &m_TempScene->mMesh[iNumMeshes]);
				iNumMeshes++;
			}
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	return S_OK;
}

HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);	
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	_bool AnimEnd = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, m_iCurrentAnimIndex);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation();
	}

	return AnimEnd;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];

	if (TYPE_ANIM == m_eModelType) 
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
			return E_FAIL;
	}

	pShader->Begin(0);
	
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_TempScene->mNumMeshes;
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, this, PivotMatrix, &m_TempScene->mMesh[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	} 

	return S_OK;
}

HRESULT CModel::Ready_Materials()
{
	if (nullptr == m_TempScene)
		return E_FAIL;

	m_iNumMaterials = m_TempScene->mNumMaterials;
	
	_uint i = 0;
	for (auto& mat : m_TempScene->mMaterials)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));
		MaterialDesc.pTexture[mat.TextureType] = CTexture::Create(m_pDevice, m_pContext, mat.mName);
		m_Materials.push_back(MaterialDesc);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(CHierarchyNode* pParent, _uint iDepth, Node* pMyNode)
{
	//AiNode의 이름과 행렬을 받아서 세팅해줌
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pParent, iDepth++, pMyNode);

	if (nullptr == pHierarchyNode)
		return E_FAIL;
	
	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pMyNode->mNumChildren; ++i)
	{
		Ready_HierarchyNodes( pHierarchyNode, iDepth, &pMyNode->mChildren[i]);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_TempScene->mNumAnimations;
	//애니메이션의 개수만큼 생성한다.
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		ANIMATION*			pMyAnimation = &m_TempScene->mAnimations[i];

		CAnimation*			pAnimation = CAnimation::Create(pMyAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}	
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _tchar* ModelFilePath, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, ModelFilePath, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*			pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	
	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();
	if (!m_bClone)
	{
		Safe_Delete(m_TempScene);
	}
	m_Importer.FreeScene();
}
