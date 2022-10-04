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
	, m_pAIScene(rhs.m_pAIScene)
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
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	Safe_AddRef(m_TempScene);

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
	HANDLE		hFile = CreateFile(ModelFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	m_TempScene = new TEMPSCENE;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	ReadFile(hFile, &m_TempScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);

	m_TempScene->mRootNode = new Node;

	LoadNode(hFile, m_TempScene->mRootNode, dwByte, dwStrByte);

	m_TempScene->mAnimations = new Animation*[m_TempScene->mNumAnimations];
	m_TempScene->mMesh = new Mesh[m_TempScene->mNumMeshes];

	//Mesh로드
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		Mesh* pMesh = &m_TempScene->mMesh[i];
		//Mesh이름 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char*	pName = nullptr;
		pName = new char[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
		pName[dwByte / sizeof(char)] = 0;
		strcpy_s(m_TempScene->mMesh[i].mName, pName);

		ReadFile(hFile, &m_TempScene->mMesh[i].mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &m_TempScene->mMesh[i].mNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &m_TempScene->mMesh[i].mNumFaces, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &m_TempScene->mMesh[i].mNumBones, sizeof(_uint), &dwByte, nullptr);

		//Vertices로드
		pMesh->mVertices = new XMFLOAT3[pMesh->mNumVertices];
		pMesh->mNormals = new XMFLOAT3[pMesh->mNumVertices];
		pMesh->mTangents = new XMFLOAT3[pMesh->mNumVertices];
		*pMesh->mTextureCoords = new XMFLOAT3[pMesh->mNumVertices];

		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			ReadFile(hFile, &pMesh->mVertices[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mNormals[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mTangents[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT3), &dwByte, nullptr);

		}


		pMesh->mBones = new Bone*[pMesh->mNumBones];
		//Bone로드
		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			pMesh->mBones[j] = new Bone;
			Bone* pBone = pMesh->mBones[j];

			//BoneName로드
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			char*	pName = nullptr;
			pName = new char[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
			pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pBone->mName, pName);

			//OffsetMatrix로드
			ReadFile(hFile, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

			//NumWeights로드
			ReadFile(hFile, &pBone->mNumWeights, sizeof(_uint), &dwByte, nullptr);

			pBone->mWeights = new VertexWeight[pBone->mNumWeights];
			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				ReadFile(hFile, &pBone->mWeights[k].mVertexId, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, &pBone->mWeights[k].mWeight, sizeof(_float), &dwByte, nullptr);
			}
		}

		//Face로드
		pMesh->mFaces = new Face[pMesh->mNumFaces];
		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[0], sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);
		}
	}

	//Material로드
	for (_uint i = 0; i < m_TempScene->mNumMaterials; ++i)
	{
		//TextureName로드
		Material* pMaterial = new Material;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		_tchar*	pName = nullptr;
		pName = new _tchar[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
		pName[dwByte / sizeof(_tchar)] = 0;
		wcscpy_s(pMaterial->mName, pName);

		ReadFile(hFile, &pMaterial->TextureType, sizeof(_uint), &dwByte, nullptr);

		m_TempScene->mMaterials.push_back(pMaterial);
	}

	//Animation로드

	m_TempScene->mAnimations = new ANIMATION*[m_TempScene->mNumAnimations];
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		m_TempScene->mAnimations[i] = new ANIMATION;
		Animation* pAnimation = m_TempScene->mAnimations[i];

		//AnimationName로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char*	pName = nullptr;
		pName = new char[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
		pName[dwByte / sizeof(char)] = 0;
		strcpy_s(pAnimation->mName, pName);

		//Animation정보로드
		ReadFile(hFile, &pAnimation->mDuration, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation->mTickPerSecond, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation->mNumChannels, sizeof(_uint), &dwByte, nullptr);

		pAnimation->mChannels = new NodeAnim*[pAnimation->mNumChannels];
		for (_uint j = 0; j < pAnimation->mNumChannels; ++j)
		{
			pAnimation->mChannels[j] = new NodeAnim;
			NodeAnim* pNodeAnim = pAnimation->mChannels[j];

			//NodeName로드
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			char*	pName = nullptr;
			pName = new char[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
			pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pNodeAnim->mNodeName, pName);

			//NumPosition로드
			ReadFile(hFile, &pNodeAnim->mNumPositionKeys, sizeof(_uint), &dwByte, nullptr);

			//PositionKey로드
			pNodeAnim->mPositionKeys = new VectorKey[pNodeAnim->mNumPositionKeys];
			for (_uint k = 0; k < pNodeAnim->mNumPositionKeys; ++k)
			{
				ReadFile(hFile, &pNodeAnim->mPositionKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				ReadFile(hFile, &pNodeAnim->mPositionKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
			}

			//NumRotation로드
			ReadFile(hFile, &pNodeAnim->mNumRotationKeys, sizeof(_uint), &dwByte, nullptr);

			//RotationKey로드
			pNodeAnim->mRotationKeys = new QuatKey[pNodeAnim->mNumRotationKeys];
			for (_uint k = 0; k < pNodeAnim->mNumRotationKeys; ++k)
			{
				ReadFile(hFile, &pNodeAnim->mRotationKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				ReadFile(hFile, &pNodeAnim->mRotationKeys[k].mValue, sizeof(XMFLOAT4), &dwByte, nullptr);
			}

			//NumScale로드
			ReadFile(hFile, &pNodeAnim->mNumScalingKeys, sizeof(_uint), &dwByte, nullptr);

			//ScalingKey로드
			pNodeAnim->mScalingKeys = new VectorKey[pNodeAnim->mNumScalingKeys];
			for (_uint k = 0; k < pNodeAnim->mNumScalingKeys; ++k)
			{
				ReadFile(hFile, &pNodeAnim->mScalingKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				ReadFile(hFile, &pNodeAnim->mScalingKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
			}
		}
	}

	if (0 == dwByte)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);
	return S_OK;
}

void CModel::LoadNode(HANDLE hFile, Node * pNode, DWORD & dwByte, DWORD & dwStrByte)
{
	ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	char*	pName = nullptr;
	pName = new char[dwStrByte];
	ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
	pName[dwByte / sizeof(char)] = 0;
	strcpy_s(pNode->mName, pName);

	ReadFile(hFile, &pNode->mNumChildren, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &pNode->mTransformation, sizeof(_float4x4), &dwByte, nullptr);

	pNode->mChildren = new Node*[pNode->mNumChildren];
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		pNode->mChildren[i] = new Node;
		LoadNode(hFile, pNode->mChildren[i], dwByte, dwStrByte);
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
	Ready_HierarchyNodes(nullptr, 0, m_TempScene->mRootNode);

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
		MaterialDesc.pTexture[mat->TextureType] = CTexture::Create(m_pDevice, m_pContext, mat->mName);
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
		Ready_HierarchyNodes( pHierarchyNode, iDepth, pMyNode->mChildren[i]);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_TempScene->mNumAnimations;
	//애니메이션의 개수만큼 생성한다.
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		ANIMATION*			pMyAnimation = m_TempScene->mAnimations[i];

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

	Safe_Release(m_TempScene);
	m_Importer.FreeScene();
}
