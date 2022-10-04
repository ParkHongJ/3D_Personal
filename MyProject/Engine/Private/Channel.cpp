#include "..\Public\Channel.h"
#include "Model.h"
#include "HierarchyNode.h"



CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(NodeAnim* pNodeAnim)
{
	/* Ư�� �ִϸ��̼ǿ��� ���Ǵ� ���� �����̴�. */
	/* �� �̸��� ���� ������ �ִ� HierarchyNodes�� ���� �� �ѳ�� �̸��� ���� ���̴�. */
	strcpy_s(m_szName, pNodeAnim->mNodeName);

	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// Safe_AddRef(m_pHierarchyNode);


	/* Ű������ �����鸦 �ε��Ѵ�. */
	/* Ű������ : ��ü�ִϸ��̼� ���� ��, Ư�� �ð��뿡 �̻��� ǥ���ؾ��� ������ ���� ��������̴�. */

	m_iNumKeyFrames = max(pNodeAnim->mNumScalingKeys, pNodeAnim->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pNodeAnim->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;
	
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if(i < pNodeAnim->mNumScalingKeys)
		{
			memcpy(&vScale, &pNodeAnim->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pNodeAnim->mScalingKeys[i].mTime;
		}
		if (i < pNodeAnim->mNumRotationKeys)
		{
			vRotation.x = pNodeAnim->mRotationKeys[i].mValue.x;
			vRotation.y = pNodeAnim->mRotationKeys[i].mValue.y;
			vRotation.z = pNodeAnim->mRotationKeys[i].mValue.z;
			vRotation.w = pNodeAnim->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = pNodeAnim->mRotationKeys[i].mTime;
		}
		if (i < pNodeAnim->mNumPositionKeys)
		{
			memcpy(&vPosition, &pNodeAnim->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pNodeAnim->mPositionKeys[i].mTime;
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

	XMStoreFloat4x4(&m_OldMatrix, TransformationMatrix);
	
	if (nullptr != pNode)
	{
		pNode->Set_Transformation(TransformationMatrix);
	}

	return iCurrentKeyFrame;
}

_uint CChannel::Blending_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	_float		fRatio = fPlayTime / .2f;

	_float3		vSourScale, vDestScale;
	_float4		vSourRotation, vDestRotation;
	_float3		vSourPosition, vDestPosition;

	_vector tempScale;
	_vector tempRotation;
	_vector tempPosition;
	
	XMMatrixDecompose(&tempScale, &tempRotation, &tempPosition, XMLoadFloat4x4(&m_OldMatrix));
	XMStoreFloat3(&vSourScale, tempScale);
	XMStoreFloat4(&vSourRotation, tempRotation);
	XMStoreFloat3(&vSourPosition, tempPosition);

	vDestScale = m_KeyFrames.front().vScale;
	vDestRotation = m_KeyFrames.front().vRotation;
	vDestPosition = m_KeyFrames.front().vPosition;

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
	XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));


	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pNode)
	{
		pNode->Set_Transformation(TransformationMatrix);
	}

	return iCurrentKeyFrame;
}

void CChannel::SetOldMatrix(_fmatrix OldMatrix)
{
	XMStoreFloat4x4(&m_OldMatrix, OldMatrix);
}

CChannel * CChannel::Create(NodeAnim* pNodeAnim)
{
	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pNodeAnim)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{

}

