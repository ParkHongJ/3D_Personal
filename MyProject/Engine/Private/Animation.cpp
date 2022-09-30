#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize_Prototype(ANIMATION* pMyAnimation)
{
	m_fDuration = pMyAnimation->mDuration;
	m_fTickPerSecond = pMyAnimation->mTickPerSecond;
	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pMyAnimation->mNumChannels;

		/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(pMyAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;		

		m_HierarchyNodes.push_back(pNode);
		
		Safe_AddRef(pNode);
	}

	return S_OK;
}


HRESULT CAnimation::Play_Animation(_float fTimeDelta, _uint& CurrentAnimIndex)
{
	if (m_bBlending)
	{
		m_fPlayTime += fTimeDelta;

		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			/*m_ChannelKeyFrames[iChannelIndex] = */pChannel->Blending_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

			++iChannelIndex;
		}

		if (m_fPlayTime >= 1.f)
		{
			m_fPlayTime = 0.f;
			m_bBlending = false;

			/*for (auto& pChannel : m_Channels)
			{
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
			}*/
		}
	}
	else
	{
		m_fPlayTime += m_fTickPerSecond * fTimeDelta;

		if (m_fPlayTime >= m_fDuration)
		{
			m_fPlayTime = 0.f;
			for (auto& pChannel : m_Channels)
			{
				for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
					iCurrentKeyFrame = 0;
			}
			//다음 애니메이션이 없다면 현재 애니메이션을 초기화해줌
			//if (nullptr == m_pNextAnimation)
			//{
			//	//반복하는 애니메이션
			//	for (auto& pChannel : m_Channels)
			//	{
			//		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			//			iCurrentKeyFrame = 0;
			//	}
			//}
			//else
			//{
			//	m_bEnd = true;
			//}
		}


		//영향을 주는 뼈들을 갱신해줌
		_uint		iChannelIndex = 0;
		
		for (auto& pChannel : m_Channels)
		{
			m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

			++iChannelIndex;
		}

		//다음 애니메이션이 있고 현재 애니메이션이 끝났다면
		/*if ( nullptr != m_pNextAnimation && m_bEnd)
		{
			++CurrentAnimIndex;
			m_pNextAnimation->Change_Animation();
			Safe_Release(m_pNextAnimation);
		}*/
	}
	return S_OK;
}

void CAnimation::Change_Animation(CAnimation* pPrevAnimation)
{
	_uint		iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->SetOldMatrix(pPrevAnimation->Get_Channel(iChannelIndex)->Get_OldMatrix());

		++iChannelIndex;
	}

	m_fPlayTime = 0.f;
	m_bBlending = true;
}

void CAnimation::TempFunc(CAnimation* pNextAnimation)
{
	//m_fPlayTime = 0.0f;
	m_pNextAnimation = pNextAnimation;
	Safe_AddRef(m_pNextAnimation);
}

void CAnimation::ResetKeyFrames()
{
	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
			iCurrentKeyFrame = 0;
	}
}

CChannel * CAnimation::Get_Channel(_uint iChannelIndex)
{
	return  m_Channels[iChannelIndex];
}

CAnimation * CAnimation::Create(ANIMATION* pMyAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pMyAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();


	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();
}
