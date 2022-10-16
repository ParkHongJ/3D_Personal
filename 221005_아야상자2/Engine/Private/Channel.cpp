#include "..\Public\Channel.h"
#include "Model.h"
#include "HierarchyNode.h"



CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(NodeAnim * pChannel)
{
	/* 특정 애니메이션에서 사용되는 뼈의 정보이다. */
	/* 이 이름은 모델이 가지고 있는 HierarchyNodes의 뼈대 중 한놈과 이름이 같을 것이다. */
	/* 이 이름으로 같은 이름을 가진 HierarchyNodes를 채널에 보관해둔다. */
	/* 왜 보관하니? : 채널이 가진 키프레임중 시간대에 맞는 키플에ㅣㅁ 상태를 만들고. 이걸로 행렬 만들고. 
	이렇게 만든 행렬을 erarchyNodes에 저장해놔야해. */
	strcpy_s(m_szName, pChannel->mNodeName);

	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// Safe_AddRef(m_pHierarchyNode);


	/* 키프레임 정보들를 로드한다. */
	/* 키프레임 : 전체애니메이션 동작 중, 특정 시간대에 이 ㅜ뼈가 표현해야할 동작의 상태 행렬정보이다. */


	m_iNumKeyFrames = pChannel->mNumKeyFrames;
	m_KeyFrames.reserve(m_iNumKeyFrames);
	copy(pChannel->mKeyFrames.begin(), pChannel->mKeyFrames.end(), back_inserter(m_KeyFrames));

	return S_OK;
}

_uint CChannel::Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode* pNode)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	/* 마지막 키프레임이상으로 넘어갔을때 : 마지막 키프레임 자세로 고정할 수 있도록 한다. */
	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;		
	}

	/* 특정 키프레임과 키프레임 사이에 존재한다. */
	/* 1을 기준으로 얼마나 재생되었는지(키프레임과 키프레임 사이를)를 확인한다.( Ratio) */
	/* 결정된Ratio값에 따라 두 키프레임사이의 상태행려를 만들어 낸다. */
	else
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;
		/*while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;*/

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
		pNode->Set_Transformation(TransformationMatrix);

	return iCurrentKeyFrame;
}

_uint CChannel::Blending_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode, _float fBlendTime)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	_float		fRatio = fPlayTime / fBlendTime;

	if (fPlayTime <= fBlendTime)
	{
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

	}
	else
	{
		vScale = m_KeyFrames.front().vScale;
		vRotation = m_KeyFrames.front().vRotation;
		vPosition = m_KeyFrames.front().vPosition;
	}

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

CChannel * CChannel::Create(NodeAnim * pChannel)
{
	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannel)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{

}

