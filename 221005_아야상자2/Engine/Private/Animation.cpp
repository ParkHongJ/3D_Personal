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
	strcpy_s(m_szName, rhs.m_szName);
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize_Prototype(Animation * pAnimation)
{
	strcpy_s(m_szName, pAnimation->mName);
	m_fDuration = pAnimation->mDuration;
	m_fTickPerSecond = pAnimation->mTickPerSecond;

	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ������ ������ �����Ѵ�. */
	m_iNumChannels = pAnimation->mNumChannels;


	/* ���� �ִϸ��̼ǿ��� �����ؾ��� ���������� �����Ͽ� �����Ѵ�. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel�� Ű������ �������� ������. */
		CChannel*		pChannel = CChannel::Create(&pAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* �� ��Ƶδµ�?> Ư�� �ִԿ��Ӽ� �����϶� �ִϸ��̼��� ����ϸ� ��� ���� ���¸� �����ϴ°� ����. ����. 
		���� �ֹ̿��ÿ��� �����ϱ����� ���븸 ���� �������ֱ� ����. */
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


_bool CAnimation::Play_Animation(_float fTimeDelta)
{
	if (m_bBlending)
	{
		m_fPlayTime += fTimeDelta;

		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			pChannel->Blending_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

			++iChannelIndex;
		}

		if (m_fPlayTime >= .2f)
		{
			m_fPlayTime = 0.f;
			m_bBlending = false;
		}
		return false;
	}
	else
	{
		_bool bAnimEnd = false;
		m_fPlayTime += m_fTickPerSecond * fTimeDelta;

		if (m_fPlayTime >= m_fDuration)
		{
			m_fPlayTime = 0.f;
			for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
				iCurrentKeyFrame = 0;
			bAnimEnd = true;
		}
		else
			bAnimEnd = false;


		//������ �ִ� ������ ��������
		/* �� �ִϸ��̼� ������ ���� ��� ������ ��ȸ�ϸ� ������ ����� �������ش�. */
		/* Transformation : ���޵� �ð��� ���� Ű������(�ð�, ������, ȸ��, �̵�)������ �̿��Ͽ� Transformation�� �����. */
		/* ���̾��Ű ��忡 �������ش�. */
		_uint		iChannelIndex = 0;

		for (auto& pChannel : m_Channels)
		{
			m_ChannelKeyFrames[iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[iChannelIndex], m_HierarchyNodes[iChannelIndex]);

			++iChannelIndex;
		}
		return bAnimEnd;
	}
}

void CAnimation::Change_Animation(CAnimation * pPrevAnimation)
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

void CAnimation::ResetKeyFrames()
{
	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
		iCurrentKeyFrame = 0;
}

CChannel * CAnimation::Get_Channel(_uint iChannelIndex)
{
	return  m_Channels[iChannelIndex];
}

CAnimation * CAnimation::Create(Animation * pAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pAnimation)))
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
