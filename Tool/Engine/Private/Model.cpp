#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"


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
	m_iCurrentAnimIndex = iAnimIndex;
	m_Animations[m_iCurrentAnimIndex]->Change_Animation();
}

_uint CModel::Get_AnimBoneSize(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex]->Get_BoneSize();
}

void CModel::TempFunc(_uint iAnimIndex, _uint iNextAnimIndex)
{
	m_Animations[m_iCurrentAnimIndex]->TempFunc(m_Animations[iNextAnimIndex]);
}

_uint CModel::GetAnimSize()
{
	return m_Animations.size();
}

HRESULT CModel::SaveFBXToBinary()
{
	HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//���� ����Ʈ 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	WriteFile(hFile, &m_TempScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_TempScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_TempScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);
	
	//Node����..
	SaveNode(hFile, m_TempScene->mRootNode, dwByte, dwStrByte);

	//Mesh����
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		Mesh* pMesh = &m_TempScene->mMesh[i];
		//�̸�����
		dwStrByte = DWORD(sizeof(char) * strlen(pMesh->mName));
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, &pMesh->mName, dwStrByte, &dwByte, nullptr);

		WriteFile(hFile, &pMesh->mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pMesh->mNumVertices, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pMesh->mNumFaces, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &pMesh->mNumBones, sizeof(_uint), &dwByte, nullptr);

		//Vertices����
		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			WriteFile(hFile, &pMesh->mVertices[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mNormals[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mTangents[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT3), &dwByte, nullptr);
		}

		int a = 10;
		//Bone����
		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			Bone* pBone = pMesh->mBones[j];
			
			//BoneName����
			dwStrByte = DWORD(sizeof(char) * strlen(pBone->mName));
			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, &pBone->mName, dwStrByte, &dwByte, nullptr);

			//OffsetMatrix����
			WriteFile(hFile, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
			
			//NumWeights����
			WriteFile(hFile, &pBone->mNumWeights, sizeof(_uint), &dwByte, nullptr);

			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				//VertexWeight����
				WriteFile(hFile, &pBone->mWeights[k].mVertexId, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, &pBone->mWeights[k].mWeight, sizeof(_float), &dwByte, nullptr);
			}
		}

		//Face����
		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			WriteFile(hFile, &pMesh->mFaces[j].mIndices[0], sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
			WriteFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);
		}
	}

	//Material����
	for (_uint i = 0; i < m_TempScene->mNumMaterials; ++i)
	{
		//Texture��� ����
		Material* pMaterial = m_TempScene->mMaterials.front();
		dwStrByte = DWORD(sizeof(_tchar) * wcslen(pMaterial->mName));
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, &pMaterial->mName, dwStrByte, &dwByte, nullptr);

		WriteFile(hFile, &pMaterial->TextureType, sizeof(_uint), &dwByte, nullptr);
		
		Safe_Delete(m_TempScene->mMaterials.front());
		m_TempScene->mMaterials.pop_front();
	}

	//Aniamtion����
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		Animation* pAnimation = m_TempScene->mAnimations[i];

		//AnimationName����
		dwStrByte = DWORD(sizeof(char) * strlen(pAnimation->mName));
		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, &pAnimation->mName, dwStrByte, &dwByte, nullptr);

		//Animation��������
		WriteFile(hFile, &pAnimation->mDuration, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &pAnimation->mTickPerSecond, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &pAnimation->mNumChannels, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < pAnimation->mNumChannels; ++j)
		{
			NodeAnim*	pNodeAnim = pAnimation->mChannels[j];

			//NodeName����
			dwStrByte = DWORD(sizeof(char) * strlen(pNodeAnim->mNodeName));
			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, &pNodeAnim->mNodeName, dwStrByte, &dwByte, nullptr);

			//NumPosition����
			WriteFile(hFile, &pNodeAnim->mNumPositionKeys, sizeof(_uint), &dwByte, nullptr);

			//PositionKey����
			for (_uint k = 0; k < pNodeAnim->mNumPositionKeys; ++k)
			{
				WriteFile(hFile, &pNodeAnim->mPositionKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				WriteFile(hFile, &pNodeAnim->mPositionKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
			}

			//NumRotation����
			WriteFile(hFile, &pNodeAnim->mNumRotationKeys, sizeof(_uint), &dwByte, nullptr);

			//RotationKey����
			for (_uint k = 0; k < pNodeAnim->mNumRotationKeys; ++k)
			{
				WriteFile(hFile, &pNodeAnim->mRotationKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				WriteFile(hFile, &pNodeAnim->mRotationKeys[k].mValue, sizeof(XMFLOAT4), &dwByte, nullptr);
			}

			//NumScale����
			WriteFile(hFile, &pNodeAnim->mNumScalingKeys, sizeof(_uint), &dwByte, nullptr);
			
			//ScalingKey����
			for (_uint k = 0; k < pNodeAnim->mNumScalingKeys; ++k)
			{
				WriteFile(hFile, &pNodeAnim->mScalingKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				WriteFile(hFile, &pNodeAnim->mScalingKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
			}
		}
	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::LoadBinary()
{
	HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	
	m_TempScene = new TEMPSCENE;
	
	//���� ����Ʈ 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;
	
	ReadFile(hFile, &m_TempScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_TempScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);

	m_TempScene->mRootNode = new Node;

	LoadNode(hFile, m_TempScene->mRootNode, dwByte, dwStrByte);

	m_TempScene->mAnimations = new Animation*[m_TempScene->mNumAnimations];
	m_TempScene->mMesh = new Mesh[m_TempScene->mNumMeshes];

	//Mesh�ε�
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		Mesh* pMesh = &m_TempScene->mMesh[i];
		//Mesh�̸� �ε�
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

		//Vertices�ε�
		pMesh->mVertices = new XMFLOAT3[pMesh->mNumVertices];
		pMesh->mNormals = new XMFLOAT3[pMesh->mNumVertices];
		pMesh->mTangents = new XMFLOAT3[pMesh->mNumVertices];
		*pMesh->mTextureCoords = new XMFLOAT3[pMesh->mNumVertices];
		
		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			ReadFile(hFile, &pMesh->mVertices[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mNormals[j], sizeof(XMFLOAT3), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mTangents[j], sizeof(XMFLOAT3), &dwByte, nullptr);
		}

		//TextureCoord�ε�
		for (_uint j = 0; j < 8; ++j)
		{
			ReadFile(hFile, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT3), &dwByte, nullptr);
		}

		pMesh->mBones = new Bone*[pMesh->mNumBones];
		//Bone�ε�
		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			pMesh->mBones[j] = new Bone;
			Bone* pBone = pMesh->mBones[j];

			//BoneName�ε�
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			char*	pName = nullptr;
			pName = new char[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
			pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pBone->mName, pName);

			//OffsetMatrix�ε�
			ReadFile(hFile, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

			//NumWeights�ε�
			ReadFile(hFile, &pBone->mNumWeights, sizeof(_uint), &dwByte, nullptr);

			pBone->mWeights = new VertexWeight[pBone->mNumWeights];
			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				ReadFile(hFile, &pBone->mWeights[k].mVertexId, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, &pBone->mWeights[k].mWeight, sizeof(_float), &dwByte, nullptr);
			}
		}

		//Face�ε�
		pMesh->mFaces = new Face[pMesh->mNumFaces];
		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[0], sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);
		}
	}

	//Material�ε�
	for (_uint i = 0; i < m_TempScene->mNumMaterials; ++i)
	{
		//TextureName�ε�
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

	//Animation�ε�

	m_TempScene->mAnimations = new ANIMATION*[m_TempScene->mNumAnimations];
	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
	{
		m_TempScene->mAnimations[i] = new ANIMATION;
		Animation* pAnimation = m_TempScene->mAnimations[i];

		//AnimationName�ε�
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		char*	pName = nullptr;
		pName = new char[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
		pName[dwByte / sizeof(char)] = 0;
		strcpy_s(pAnimation->mName, pName);

		//Animation�����ε�
		ReadFile(hFile, &pAnimation->mDuration, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation->mTickPerSecond, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &pAnimation->mNumChannels, sizeof(_uint), &dwByte, nullptr);

		pAnimation->mChannels = new NodeAnim*[pAnimation->mNumChannels];
		for (_uint j = 0; j < pAnimation->mNumChannels; ++j)
		{
			pAnimation->mChannels[j] = new NodeAnim;
			NodeAnim* pNodeAnim = pAnimation->mChannels[j];

			//NodeName�ε�
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			char*	pName = nullptr;
			pName = new char[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);
			pName[dwByte / sizeof(char)] = 0;
			strcpy_s(pNodeAnim->mNodeName, pName);

			//NumPosition�ε�
			ReadFile(hFile, &pNodeAnim->mNumPositionKeys, sizeof(_uint), &dwByte, nullptr);

			//PositionKey�ε�
			pNodeAnim->mPositionKeys = new VectorKey[pNodeAnim->mNumPositionKeys];
			for (_uint k = 0; k < pNodeAnim->mNumPositionKeys; ++k)
			{
				ReadFile(hFile, &pNodeAnim->mPositionKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				ReadFile(hFile, &pNodeAnim->mPositionKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
			}

			//NumRotation�ε�
			ReadFile(hFile, &pNodeAnim->mNumRotationKeys, sizeof(_uint), &dwByte, nullptr);

			//RotationKey�ε�
			pNodeAnim->mRotationKeys = new QuatKey[pNodeAnim->mNumRotationKeys];
			for (_uint k = 0; k < pNodeAnim->mNumRotationKeys; ++k)
			{
				ReadFile(hFile, &pNodeAnim->mRotationKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
				ReadFile(hFile, &pNodeAnim->mRotationKeys[k].mValue, sizeof(XMFLOAT4), &dwByte, nullptr);
			}

			//NumScale�ε�
			ReadFile(hFile, &pNodeAnim->mNumScalingKeys, sizeof(_uint), &dwByte, nullptr);

			//ScalingKey�ε�
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

void CModel::SaveNode(HANDLE hFile, Node * pNode, DWORD& dwByte, DWORD& dwStrByte)
{
	//NodeName����
	dwStrByte = DWORD(sizeof(char) * strlen(pNode->mName));
	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	WriteFile(hFile, &pNode->mName, dwStrByte, &dwByte, nullptr);

	//mNumChildren����
	WriteFile(hFile, &pNode->mNumChildren, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &pNode->mTransformation, sizeof(_float4x4), &dwByte, nullptr);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		SaveNode(hFile, pNode->mChildren[i], dwByte, dwStrByte);
	}
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

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{

	//LoadBinary();
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	m_eModelType = eType;

	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	else
		iFlag |= aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;

	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	m_TempScene = new TEMPSCENE;
	int a = 10;
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/*Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest) 
	{	
		return pSour->Get_Depth() < pDest->Get_Depth();
	});*/

	/* ���� �����ϴ� �޽õ��� �����. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;


	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	m_TempScene->mRootNode = new Node;
	//strcpy_s(m_TempScene->mRootNode->mParent->mName, "");
	Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0, m_TempScene->mRootNode);

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
				pMeshContainer->SetUp_HierarchyNodes(this, m_pAIScene->mMeshes[iNumMeshes], &m_TempScene->mMesh[iNumMeshes]);
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

	//SaveFBXToBinary();
	return S_OK;
}

HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);	
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* ���� ����ϰ����ϴ� �ִϸ��̼��� �����ؾ��� ������ ��������� �����س���. */
	m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, m_iCurrentAnimIndex);

	/* ��������� ����������(�θ𿡼� �ڽ�����) �����Ͽ� m_CombinedTransformation�� �����.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation();
	}

	return S_OK;
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
	m_iNumMeshes = m_pAIScene->mNumMeshes;
	m_TempScene->mNumMeshes = m_pAIScene->mNumMeshes;
	m_TempScene->mMesh = new Mesh[m_TempScene->mNumMeshes];
	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix, &m_TempScene->mMesh[i]);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	} 

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;
	m_TempScene->mNumMaterials = m_pAIScene->mNumMaterials;
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		aiMaterial*			pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{			
			aiString		strPath;			

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;		

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szWideFullPath, MAX_PATH);

			//m_Path.push_back(szWideFullPath);
			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			
			Material* pMaterial = new Material;
			wcscpy_s(pMaterial->mName, szWideFullPath);
			pMaterial->TextureType = j;
			m_TempScene->mMaterials.push_back(pMaterial);
			
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;			
		}	

		m_Materials.push_back(MaterialDesc);
	}

	//HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	//HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	//if (INVALID_HANDLE_VALUE == hFile)
	//	return S_OK;

	//DWORD	dwByte = 0;
	//DWORD	dwStrByte = 0;
	//_tchar	szWideFullPath[MAX_PATH] = TEXT("");

	////for (auto& iter : m_Path)
	////{
	////	// Key�� ����
	////	dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter.c_str()));
	////	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	////	WriteFile(hFile, iter.c_str(), dwStrByte, &dwByte, nullptr);
	////}
	//while (true)
	//{
	//	ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	//	_tchar*	pFirst = nullptr;
	//	pFirst = new _tchar[dwStrByte];
	//	ReadFile(hFile, pFirst, dwStrByte, &dwByte, nullptr);
	//	pFirst[dwByte / sizeof(_tchar)] = 0;
	//	if (0 == dwByte)
	//	{
	//		break;
	//	}
	//	m_TexturePath.push_back(pFirst);
	//}
	//CloseHandle(hFile);

	//auto iter = m_pObjects.begin();

	//while (iter != m_pObjects.end())
	//{
	//	// Key�� ����
	//	dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter->first->pPrototypeTag.c_str()));
	//	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	//	WriteFile(hFile, iter->first->pPrototypeTag.c_str(), dwStrByte, &dwByte, nullptr);
	//	// Key�� ����
	//	dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter->first->pPrototypeTag.c_str()));
	//	WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	//	WriteFile(hFile, iter->first->pLayerTag.c_str(), dwStrByte, &dwByte, nullptr);

	//	//Second�� ����
	//	WriteFile(hFile, iter->second->vPos, sizeof(_float3), &dwByte, nullptr);
	//	WriteFile(hFile, &iter->second->iNumLevel, sizeof(_uint), &dwByte, nullptr);
	//	WriteFile(hFile, &iter->second->iDirection, sizeof(_uint), &dwByte, nullptr);
	//	WriteFile(hFile, &iter->second->iTex, sizeof(_uint), &dwByte, nullptr);
	//	++iter;
	//}
	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth, Node* pMyNode)
{
	//AiNode�� �̸��� ����� �޾Ƽ� ��������
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++, pMyNode);

	if (nullptr == pHierarchyNode)
		return E_FAIL;
	
	m_HierarchyNodes.push_back(pHierarchyNode);

	pMyNode->mNumChildren = pNode->mNumChildren;
	pMyNode->mChildren = new Node*[pMyNode->mNumChildren];

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		pMyNode->mChildren[i] = new Node;
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth, pMyNode->mChildren[i]);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;
	m_TempScene->mNumAnimations = m_pAIScene->mNumAnimations;

	m_TempScene->mAnimations = new ANIMATION*[m_TempScene->mNumAnimations];
	//�ִϸ��̼��� ������ŭ �����Ѵ�.
	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];
		m_TempScene->mAnimations[i] = new ANIMATION;
		ANIMATION*			pMyAnimation = m_TempScene->mAnimations[i];

		CAnimation*			pAnimation = CAnimation::Create(pAIAnimation, pMyAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}	
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pModelFileName, PivotMatrix)))
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

	delete m_TempScene;

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

	m_Importer.FreeScene();
}
