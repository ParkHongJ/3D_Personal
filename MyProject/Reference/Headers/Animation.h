#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(ANIMATION* pMyAnimation);
	HRESULT Initialize(class CModel* pModel);
	_bool Play_Animation(_float fTimeDelta, _uint& CurrentAnimIndex);
	_uint Get_BoneSize() { return m_Channels.size(); }
	void Change_Animation(CAnimation* pPrevAnimation);
	void ResetKeyFrames();
	class CChannel* Get_Channel(_uint iChannelIndex);
private:
	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;

private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	
	vector<_uint>					m_ChannelKeyFrames;
	_bool							m_bBlending = false;

	CAnimation*						m_pNextAnimation = nullptr;

	_bool							m_bLooping = true;
	_bool							m_bEnd = false;
public:
	static CAnimation* Create(ANIMATION* pMyAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END