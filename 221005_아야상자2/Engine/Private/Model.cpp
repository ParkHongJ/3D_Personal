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


	/* ���� �����ϴ� �޽õ��� �����. */
	/* ���� �������� �޽÷� �����Ǿ��ִ�. */
	/* �� �޽��� ������� �ε������� �����Ѵ�. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	/* ���׸���������.(���� �޾����� �����ؾ��� ��������.) */
	/* �𵨸�������?, ������������? �ȼ����� ����(o) �ؽ��ķ� ǥ���ȴ�. */
	if (FAILED(Ready_Materials()))
		return E_FAIL;


	/* �ִϸ��̼��� ������ �о �����Ѵ�.  */
	/* �ִϸ��̼� ���� : �ִϸ��̼��� ����Ǵµ� �ɸ��� �� �ð�(Duration),  �ִϸ��̼��� ����ӵ�( mTickPerSecond), ��� ä��(mNumChannels) �� ���⸣ �ִ°�. ��ä���� ����(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, �ִϸ��̼��� �����̴� ��) �� ���� ������ �����Ͽ� ��üȭ�Ѵ�.(CChannel) */
	/* ä�� : ��. �� ���� �� �ִϸ��̼� �ȿ��� ���ȴ�. �� �ִϸ��̼� �ȿ��� � �ð�, �ð�, �ð�, �ð��뿡 � ���¸� ǥ���ϸ� �Ǵ����� ���� ����(keyframe)���� �ٹǳ�. */
	/* keyframe : ��ð�?, ����(vScale, vRotation, vPosition) */
	
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


	/* ���� �����ϴ� �޽õ��� �����. */
	/* ���� �������� �޽÷� �����Ǿ��ִ�. */
	/* �� �޽��� ������� �ε������� �����Ѵ�. */
	if (FAILED(Ready_InstanceMeshContainers(iNumInstance, PivotMatrix)))
		return E_FAIL;

	/* ���׸���������.(���� �޾����� �����ؾ��� ��������.) */
	/* �𵨸�������?, ������������? �ȼ����� ����(o) �ؽ��ķ� ǥ���ȴ�. */
	if (FAILED(Ready_Materials()))
		return E_FAIL;


	/* �ִϸ��̼��� ������ �о �����Ѵ�.  */
	/* �ִϸ��̼� ���� : �ִϸ��̼��� ����Ǵµ� �ɸ��� �� �ð�(Duration),  �ִϸ��̼��� ����ӵ�( mTickPerSecond), ��� ä��(mNumChannels) �� ���⸣ �ִ°�. ��ä���� ����(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, �ִϸ��̼��� �����̴� ��) �� ���� ������ �����Ͽ� ��üȭ�Ѵ�.(CChannel) */
	/* ä�� : ��. �� ���� �� �ִϸ��̼� �ȿ��� ���ȴ�. �� �ִϸ��̼� �ȿ��� � �ð�, �ð�, �ð�, �ð��뿡 � ���¸� ǥ���ϸ� �Ǵ����� ���� ����(keyframe)���� �ٹǳ�. */
	/* keyframe : ��ð�?, ����(vScale, vRotation, vPosition) */

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

	//���� ����Ʈ 
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
	//Mesh�ε�
	for (_uint i = 0; i < m_pHScene->mNumMeshes; ++i)
	{
		Mesh pMesh;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pMesh.mName, dwStrByte, &dwByte, nullptr);

		ReadFile(hFile, &pMesh.mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pMesh.mNumFaces, sizeof(_uint), &dwByte, nullptr);

		//Vertices�ε�
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
			//Bone�ε�
			pMesh.mBones.reserve(pMesh.mNumBones);
			for (_uint j = 0; j < pMesh.mNumBones; ++j)
			{
				Bone pBone;

				//BoneName�ε�
				ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				ReadFile(hFile, pBone.mName, dwStrByte, &dwByte, nullptr);

				//OffsetMatrix�ε�
				ReadFile(hFile, &pBone.mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

				//NumWeights�ε�
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
		//Face�ε�
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

	//Material�ε�
	for (_uint i = 0; i < m_pHScene->mNumMaterials; ++i)
	{
		//TextureName�ε�
		Material pMaterial;
		ZeroMemory(&pMaterial, sizeof(Material));
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, pMaterial.mName, dwStrByte, &dwByte, nullptr);

		ReadFile(hFile, &pMaterial.TextureType, sizeof(_uint), &dwByte, nullptr);

		m_pHScene->mMaterials.push_back(pMaterial);
	}

	if (m_eModelType == TYPE_ANIM)
	{
		//Animation�ε�
		m_pHScene->mAnimations.reserve(m_pHScene->mNumAnimations);
		for (_uint i = 0; i < m_pHScene->mNumAnimations; ++i)
		{
			Animation pAnimation;

			//AnimationName�ε�
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			ReadFile(hFile, pAnimation.mName, dwStrByte, &dwByte, nullptr);

			//Animation�����ε�
			ReadFile(hFile, &pAnimation.mDuration, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &pAnimation.mTickPerSecond, sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &pAnimation.mNumChannels, sizeof(_uint), &dwByte, nullptr);

			pAnimation.mChannels.reserve(pAnimation.mNumChannels);
			for (_uint j = 0; j < pAnimation.mNumChannels; ++j)
			{
				NodeAnim pNodeAnim;

				//NodeName�ε�
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
	/* ���� ������ �ε��ϳ�. */
	/* �� �� ��ü�� ���� ������ �ε��Ѵ�. */
	/* HierarchyNode : ���� ���¸� ������.(offSetMatrix, Transformation, CombinedTransformation */
	if (TYPE_ANIM == m_eModelType)
	{
		Ready_HierarchyNodes(&m_pHScene->mRootNode, nullptr, 0);

		/* ������ �����Ѵ�. */
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
	/* ���� ����ϰ����ϴ� �ִϸ��̼��� �����ؾ��� ������ ��������� �����س���. */
	_bool AnimEnd = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta);

	/* ��������� ����������(�θ𿡼� �ڽ�����) �����Ͽ� m_CombinedTransformation�� �����.  */
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

		/* �� ������ ��Ű��. */
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
	/* �޽��� ������ ���´�. */
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
	/* �޽��� ������ ���´�. */
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

	/* �� ���� ��� ���׸��� ������ �̿��ϴ°�. */
	/* ���׸���(MATERIALDESC) : �ؽ���[��ǻ��or�ں��Ʈor�븻or�̹̽ú� ���� ] */
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
	/* pParent? : �θ� ��� �ּ�. CombinedTransformation���� �׸���. 
	CombinedTransformation���� ������� ���� Transformation * �θ���CombinedTranfsormation�� �����. */
	/* �׷��� �θ� �ʿ���. */
	/* iDepth? : Ready_HierarchyNodes�Լ��� ������·� �θ�����ֱ⤨�Ť̤��� ����(����)���� �����س����� ������. */
	/* ���� �ִԿ��Ӽ� ����Ҷ� ���� CombinedTransformation�� �����. */
	/* CombinedTransformation������� �θ��� ���°� ��� ���ŵǾ��־�ߵ�. �� �θ��� �Ĺ��ε� �̿��ϴϱ�.�� == 
	 �θ���� �ڽ����� ���������� CombinedTransformation�� �������Ѵٶ�� �� �ǹ�.  */
	/* m_HierarchyNodes�����̳ʴ� �ֻ��� �θ� ���� �տ� �־���Ѵ�. �̳��� 1�� �ڽĵ��� �ι�°�� ��. ����¥�ĵ��� �״����Ҿ�. */
	/* �� ��帶�� ���̰�(�����ڽ��̳�? ) �� �����صΰ� ���߿� �����Ѵ�. */
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

		/*I �ִϸ��̼� ���� ��üȭ �ϴ� ���� : ���� ��� �ð��� �´� ä�ε��� �� ���¸� �����Ѵ�. (���� ������) 
		�Լ��� ����������. */
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
