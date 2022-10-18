#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"
#include "MeshContainerInstance.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_InstanceMeshes(rhs.m_InstanceMeshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_pHScene(rhs.m_pHScene)
	, m_bClone(true)
	
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);

	for (auto& pMeshContainer : m_InstanceMeshes)
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
	if (m_eModelType == CModel::TYPE_INSTANCE_NONANIM)
	{
		return m_InstanceMeshes[iMeshIndex]->Get_MaterialIndex();
	}
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar * pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eType;
	LoadBinary(pModelFilePath);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);


	/* 모델을 구성하는 메시들을 만든다. */
	/* 모델은 여러개의 메시로 구성되어있다. */
	/* 각 메시의 정점들과 인덱스들을 구성한다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	/* 머테리얼정보다.(빛을 받았을때 리턴해야할 색상정보.) */
	/* 모델마다정의?, 정점마다정의? 픽셀마다 정의(o) 텍스쳐로 표현된다. */
	if (FAILED(Ready_Materials()))
		return E_FAIL;


	/* 애니메이션의 정보를 읽어서 저장한다.  */
	/* 애니메이션 정보 : 애니메이션이 재생되는데 걸리는 총 시간(Duration),  애니메이션의 재생속도( mTickPerSecond), 몇개의 채널(mNumChannels) 에 영향르 주는가. 각채널의 정보(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, 애니메이션이 움직이는 뼈) 에 대한 정보를 구성하여 객체화한다.(CChannel) */
	/* 채널 : 뼈. 이 뼈는 한 애니메이션 안에서 사용된다. 그 애니메이션 안에서 어떤 시간, 시간, 시간, 시간대에 어떤 상태를 표현하면 되는지에 대한 정보(keyframe)들을 다므낟. */
	/* keyframe : 어떤시간?, 상태(vScale, vRotation, vPosition) */
	
	if (m_eModelType == TYPE_ANIM)
		if (FAILED(Ready_Animations()))
			return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _tchar * pModelFilePath, _uint iNumInstance, _fmatrix PivotMatrix)
{
	m_eModelType = eType;
	LoadBinary(pModelFilePath);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);


	/* 모델을 구성하는 메시들을 만든다. */
	/* 모델은 여러개의 메시로 구성되어있다. */
	/* 각 메시의 정점들과 인덱스들을 구성한다. */
	if (FAILED(Ready_InstanceMeshContainers(iNumInstance, PivotMatrix)))
		return E_FAIL;

	/* 머테리얼정보다.(빛을 받았을때 리턴해야할 색상정보.) */
	/* 모델마다정의?, 정점마다정의? 픽셀마다 정의(o) 텍스쳐로 표현된다. */
	if (FAILED(Ready_Materials()))
		return E_FAIL;


	/* 애니메이션의 정보를 읽어서 저장한다.  */
	/* 애니메이션 정보 : 애니메이션이 재생되는데 걸리는 총 시간(Duration),  애니메이션의 재생속도( mTickPerSecond), 몇개의 채널(mNumChannels) 에 영향르 주는가. 각채널의 정보(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, 애니메이션이 움직이는 뼈) 에 대한 정보를 구성하여 객체화한다.(CChannel) */
	/* 채널 : 뼈. 이 뼈는 한 애니메이션 안에서 사용된다. 그 애니메이션 안에서 어떤 시간, 시간, 시간, 시간대에 어떤 상태를 표현하면 되는지에 대한 정보(keyframe)들을 다므낟. */
	/* keyframe : 어떤시간?, 상태(vScale, vRotation, vPosition) */

	if (m_eModelType == TYPE_ANIM)
		if (FAILED(Ready_Animations()))
			return E_FAIL;

	return S_OK;
}

_bool CModel::RecvMessage(const char * Message)
{
	//return m_Animations[m_iCurrentAnimIndex]->RecvMessage(Message);
	return true;
}

vector<class CAnimation*>* CModel::GetAnimations()
{
	if (!m_Animations.empty())
		return &m_Animations;
	else
		return nullptr;
}

void CModel::Change_Animation(_uint iAnimIndex, _float fBlendTime, _bool bLoop)
{
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		_uint iPrevAnimIndex = m_iCurrentAnimIndex;
		m_iCurrentAnimIndex = iAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->ResetKeyFrames();
		m_Animations[m_iCurrentAnimIndex]->SetBlendTimeAndLoop(fBlendTime, bLoop);
		m_Animations[m_iCurrentAnimIndex]->Change_Animation(m_Animations[iPrevAnimIndex]);
		m_Animations[iPrevAnimIndex]->ResetKeyFrames();
	}
}

HRESULT CModel::LoadBinary(const _tchar * ModelFilePath)
{
	HANDLE		hFile = CreateFile(ModelFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	m_pHScene = new HSCENE;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	ReadFile(hFile, &m_pHScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_pHScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);

	if (m_eModelType == TYPE_ANIM)
	{
		ReadFile(hFile, &m_pHScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);

		LoadNode(hFile, &m_pHScene->mRootNode, dwByte, dwStrByte);
	}
	m_pHScene->mMesh.reserve(m_pHScene->mNumMeshes);
	//Mesh로드
	for (_uint i = 0; i < m_pHScene->mNumMeshes; ++i)
	{
		Mesh pMesh;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pMesh.mName, dwStrByte, &dwByte, nullptr);

		ReadFile(hFile, &pMesh.mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumFaces, sizeof(_uint), &dwByte, nullptr);

		//Vertices로드
		pMesh.mVertices.reserve(pMesh.mNumVertices);
		for (_uint j = 0; j < pMesh.mNumVertices; ++j)
		{
			VerticesInfo vInfo;
			ZeroMemory(&vInfo, sizeof(VerticesInfo));
			ReadFile(hFile, &vInfo, sizeof(VerticesInfo), &dwByte, nullptr);
			pMesh.mVertices.push_back(vInfo);
		}
		if (m_eModelType == TYPE_ANIM)
		{
			ReadFile(hFile, &pMesh.mNumBones, sizeof(_uint), &dwByte, nullptr);
			//Bone로드
			pMesh.mBones.reserve(pMesh.mNumBones);
			for (_uint j = 0; j < pMesh.mNumBones; ++j)
			{
				Bone pBone;

				//BoneName로드
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				ReadFile(hFile, pBone.mName, dwStrByte, &dwByte, nullptr);

				//OffsetMatrix로드
				ReadFile(hFile, &pBone.mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

				//NumWeights로드
				ReadFile(hFile, &pBone.mNumWeights, sizeof(_uint), &dwByte, nullptr);

				pBone.mWeights.reserve(pBone.mNumWeights);
				for (_uint k = 0; k < pBone.mNumWeights; ++k)
				{
					VertexWeight Weight;
					ZeroMemory(&Weight, sizeof(VertexWeight));

					ReadFile(hFile, &Weight, sizeof(VertexWeight), &dwByte, nullptr);

					pBone.mWeights.push_back(Weight);
				}
				pMesh.mBones.push_back(pBone);
			}

		}
		//Face로드
		pMesh.mFaces.reserve(pMesh.mNumFaces);
		for (_uint j = 0; j < pMesh.mNumFaces; ++j)
		{
			FACEINDICES32 mFace;
			ZeroMemory(&mFace, sizeof(FACEINDICES32));
			ReadFile(hFile, &mFace, sizeof(FACEINDICES32), &dwByte, nullptr);
			pMesh.mFaces.push_back(mFace);
		}
		m_pHScene->mMesh.push_back(pMesh);
	}

	//Material로드
	for (_uint i = 0; i < m_pHScene->mNumMaterials; ++i)
	{
		//TextureName로드
		Material pMaterial;
		ZeroMemory(&pMaterial, sizeof(Material));
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pMaterial.mName, dwStrByte, &dwByte, nullptr);

		ReadFile(hFile, &pMaterial.TextureType, sizeof(_uint), &dwByte, nullptr);

		m_pHScene->mMaterials.push_back(pMaterial);
	}

	if (m_eModelType == TYPE_ANIM)
	{
		//Animation로드
		m_pHScene->mAnimations.reserve(m_pHScene->mNumAnimations);
		for (_uint i = 0; i < m_pHScene->mNumAnimations; ++i)
		{
			Animation pAnimation;

			//AnimationName로드
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, pAnimation.mName, dwStrByte, &dwByte, nullptr);

			//Animation정보로드
			ReadFile(hFile, &pAnimation.mDuration, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &pAnimation.mTickPerSecond, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &pAnimation.mNumChannels, sizeof(_uint), &dwByte, nullptr);

			pAnimation.mChannels.reserve(pAnimation.mNumChannels);
			for (_uint j = 0; j < pAnimation.mNumChannels; ++j)
			{
				NodeAnim pNodeAnim;

				//NodeName로드
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				ReadFile(hFile, pNodeAnim.mNodeName, dwStrByte, &dwByte, nullptr);

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
			m_pHScene->mAnimations.push_back(pAnimation);
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
	ReadFile(hFile, pNode->mName, dwStrByte, &dwByte, nullptr);

	ReadFile(hFile, &pNode->mNumChildren, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &pNode->mTransformation, sizeof(_float4x4), &dwByte, nullptr);

	pNode->mChildren.reserve(pNode->mNumChildren);
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Node ChildNode;
		LoadNode(hFile, &ChildNode, dwByte, dwStrByte);
		pNode->mChildren.push_back(ChildNode);
	}
}

HRESULT CModel::Initialize(void * pArg)
{
	/* 뼈대 정볼르 로드하낟. */
	/* 이 모델 전체의 뼈의 정보를 로드한다. */
	/* HierarchyNode : 뼈의 상태를 가진다.(offSetMatrix, Transformation, CombinedTransformation */
	if (TYPE_ANIM == m_eModelType)
	{
		Ready_HierarchyNodes(&m_pHScene->mRootNode, nullptr, 0);

		/* 뎁스로 정렬한다. */
		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});


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
				pMeshContainer->SetUp_HierarchyNodes(this, &m_pHScene->mMesh[iNumMeshes]);
			iNumMeshes++;
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
	_bool AnimEnd = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation();
	}

	return AnimEnd;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	_float4x4		BoneMatrices[256];

	if (TYPE_ANIM == m_eModelType) 
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		/* 모델 정점의 스키닝. */
		if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
			return E_FAIL;
	}

	pShader->Begin(iPassIndex);
	
	if (m_eModelType != TYPE_INSTANCE_NONANIM)
	{
		m_Meshes[iMeshIndex]->Render();
	}
	else
	{
		m_InstanceMeshes[iMeshIndex]->Render();
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	/* 메시의 갯수를 얻어온다. */
	m_iNumMeshes = m_pHScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, &m_pHScene->mMesh[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_InstanceMeshContainers(_uint iNumInstance, _fmatrix PivotMatrix)
{
	/* 메시의 갯수를 얻어온다. */
	m_iNumMeshes = m_pHScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainerInstance*		pMeshContainer = CMeshContainerInstance::Create(m_pDevice, m_pContext, iNumInstance, &m_pHScene->mMesh[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_InstanceMeshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials()
{
	if (nullptr == m_pHScene)
		return E_FAIL;

	/* 이 모델은 몇개의 머테리얼 정보를 이용하는가. */
	/* 머테리얼(MATERIALDESC) : 텍스쳐[디퓨즈or앰비언트or노말or이미시브 등등등 ] */
	m_iNumMaterials = m_pHScene->mNumMaterials;

	for (auto& mat : m_pHScene->mMaterials)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));
		MaterialDesc.pTexture[mat.TextureType] = CTexture::Create(m_pDevice, m_pContext, mat.mName);
		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(Node* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	/* pParent? : 부모 노드 주소. CombinedTransformation으로 그린다. 
	CombinedTransformation놈을 만들려면 나의 Transformation * 부모의CombinedTranfsormation로 만든다. */
	/* 그래서 부모가 필요해. */
	/* iDepth? : Ready_HierarchyNodes함수를 재귀형태로 부르고ㅓ있기ㄸ매ㅜㄴ에 한쪽(깊이)으로 생성해나가기 때문에. */
	/* 이후 애님에ㅣ션 재생할때 뼈의 CombinedTransformation를 만든다. */
	/* CombinedTransformation만들려면 부모의 상태가 모두 갱신되어있어야돼. 왜 부모의 컴바인드 이용하니까.ㄴ == 
	 부모부터 자식으로 순차적으로 CombinedTransformation를 만들어야한다라는 걸 의미.  */
	/* m_HierarchyNodes컨테이너는 최상위 부모가 가장 앞에 있어야한다. 이놈의 1차 자식들이 두번째에 쫙. 삼차짜식들이 그다음쫘악. */
	/* 각 노드마다 깊이값(몇차자식이냐? ) 을 저장해두고 나중에 정렬한다. */
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++);

	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_HierarchyNodes(&pNode->mChildren[i], pHierarchyNode, iDepth);
	}	

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pHScene->mNumAnimations;

	for (_uint i = 0; i < m_pHScene->mNumAnimations; ++i)
	{
		ANIMATION*		pMyAnimation = &m_pHScene->mAnimations[i];

		/*I 애니메이션 마다 객체화 하는 이유 : 현재 재생 시간에 맞는 채널들의 뼈 상태를 셋팅한다. (조난 빡세다) 
		함수로 만들어야지뭐. */
		CAnimation*			pAnimation = CAnimation::Create(pMyAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}	
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _tchar * pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _tchar * pModelFilePath, _uint iNumInstance, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, iNumInstance, PivotMatrix)))
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

	if (!m_bClone)
		Safe_Delete(m_pHScene);
	
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

	for (auto& pMeshContainer : m_InstanceMeshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();


	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

}
