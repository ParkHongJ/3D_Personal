#include "..\Public\Channel.h"
#include "Model.h"
#include "HierarchyNode.h"



CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, NodeAnim* pNodeAnim)
{
	/* Ư�� �ִϸ��̼ǿ��� ���Ǵ� ���� �����̴�. */
	/* �� �̸��� ���� ������ �ִ� HierarchyNodes�� ���� �� �ѳ�� �̸��� ���� ���̴�. */
	strcpy_s(m_szName, pAIChannel->mNodeName.data);
	strcpy_s(pNodeAnim->mNodeName, pAIChannel->mNodeName.data);
	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// Safe_AddRef(m_pHierarchyNode);


	/* Ű������ �����鸦 �ε��Ѵ�. */
	/* Ű������ : ��ü�ִϸ��̼� ���� ��, Ư�� �ð��뿡 �̻��� ǥ���ؾ��� ������ ���� ��������̴�. */
	pNodeAnim->mNumPositionKeys = pAIChannel->mNumPositionKeys;
	pNodeAnim->mNumRotationKeys = pAIChannel->mNumRotationKeys;
	pNodeAnim->mNumScalingKeys = pAIChannel->mNumScalingKeys;

	pNodeAnim->mPositionKeys = new VECTORKEY[pNodeAnim->mNumPositionKeys];
	pNodeAnim->mRotationKeys = new QuatKey[pNodeAnim->mNumRotationKeys];
	pNodeAnim->mScalingKeys = new VECTORKEY[pNodeAnim->mNumScalingKeys];

	for (_uint i = 0; i < pNodeAnim->mNumPositionKeys; ++i)
	{
		pNodeAnim->mPositionKeys[i].mTime = pAIChannel->mPositionKeys[i].mTime;
		memcpy(&pNodeAnim->mPositionKeys[i].mValue, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
	}
	for (_uint i = 0; i < pNodeAnim->mNumRotationKeys; ++i)
	{
		pNodeAnim->mRotationKeys[i].mTime = pAIChannel->mRotationKeys[i].mTime;
		pNodeAnim->mRotationKeys[i].mValue.x = pAIChannel->mRotationKeys[i].mValue.x;
		pNodeAnim->mRotationKeys[i].mValue.y = pAIChannel->mRotationKeys[i].mValue.y;
		pNodeAnim->mRotationKeys[i].mValue.z = pAIChannel->mRotationKeys[i].mValue.z;
		pNodeAnim->mRotationKeys[i].mValue.w = pAIChannel->mRotationKeys[i].mValue.w;
	}
	for (_uint i = 0; i < pNodeAnim->mNumScalingKeys; ++i)
	{
		pNodeAnim->mScalingKeys[i].mTime = pAIChannel->mScalingKeys[i].mTime;
		memcpy(&pNodeAnim->mScalingKeys[i].mValue, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
	}

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;
	
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if(i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mScalingKeys[i].mTime;
		}
		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = pAIChannel->mRotationKeys[i].mTime;
		}
		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}	

	return S_OK;
}

_uint CChannel::Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode* pNode)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;		
	}
	else
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));	

	if (nullptr != pNode)
	{
		pNode->Set_Transformation(TransformationMatrix);
	}

	return iCurrentKeyFrame;
}

_uint CChannel::Blending_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode, CChannel* pNextChannel)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	if (fPlayTime >= .2f)
	{
		vScale = m_KeyFrames.front().vScale;
		vRotation = m_KeyFrames.front().vRotation;
		vPosition = m_KeyFrames.front().vPosition;
	}
	else
	{
		while (fPlayTime >= .2f)
		{
			KEYFRAME NextKeyFrame = pNextChannel->GetFirstKeyFrame();
			_matrix		TransformationMatrix =
				XMMatrixAffineTransformation(
					XMLoadFloat3(&NextKeyFrame.vScale),
					XMVectorSet(0.f, 0.f, 0.f, 1.f),
					XMLoadFloat4(&NextKeyFrame.vRotation),
					XMVectorSetW(XMLoadFloat3(&NextKeyFrame.vPosition), 1.f));

			if (nullptr != pNode)
			{
				pNode->Set_Transformation(TransformationMatrix);
			}

			return 0;
		}

		_float		fRatio = fPlayTime / .2f;

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		/*_vector tempScale;
		_vector tempRotation;
		_vector tempPosition;*/
		
		/*XMMatrixDecompose(&tempScale, &tempRotation, &tempPosition, XMLoadFloat4x4(&m_OldMatrix));
		
		XMStoreFloat3(&vSourScale, tempScale);
		XMStoreFloat4(&vSourRotation, tempRotation);
		XMStoreFloat3(&vSourPosition, tempPosition);*/

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;

		KEYFRAME NextKeyframe = pNextChannel->GetFirstKeyFrame();
		NextKeyframe.vPosition;

		vDestScale = NextKeyframe.vScale;
		vDestRotation = NextKeyframe.vRotation;
		vDestPosition = NextKeyframe.vPosition;

		/*vDestScale = m_KeyFrames.front().vScale;
		vDestRotation = m_KeyFrames.front().vRotation;
		vDestPosition = m_KeyFrames.front().vPosition;*/

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pNode)
	{
		pNode->Set_Transformation(TransformationMatrix);
	}

	return 0;
}

void CChannel::SetOldMatrix(CHierarchyNode * pNode)
{
	XMStoreFloat4x4(&m_OldMatrix, pNode->Get_OldTransformation());
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, NodeAnim* pNodeAnim)
{
	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pNodeAnim)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{

}

