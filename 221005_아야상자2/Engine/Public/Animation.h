#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(Animation* pAnimation);
	HRESULT Initialize(class CModel* pModel);
	_bool Play_Animation(_float fTimeDelta);
	void Change_Animation(CAnimation* pPrevAnimation);
	void ResetKeyFrames();

	class CChannel* Get_Channel(_uint iChannelIndex);

	const char* GetName() {
		return m_szName;
	}
	void SetBlendTimeAndLoop(_float fBlendTime, _bool bLoop) {
		m_fBlendTime = fBlendTime; 
		m_bLoop = bLoop;
	}
	/* For. Imgui */
	void AddMessageInfo(const char* Message, _bool bLoop, _bool bHasExitTime, _uint iNextAnimIndex)
	{
		m_bLoop = bLoop;
		m_bHasExitTime = bHasExitTime;
		m_Message.insert({Message, iNextAnimIndex});
	}
	
	/* For. Imgui */
	void GetAnimationInfo(_float& fBlendTime, _bool& bLoop, _bool& bHasExitTime)
	{
		fBlendTime = m_fBlendTime;
		bLoop = m_bLoop;
		bHasExitTime = m_bHasExitTime;
	}

	/* For. Imgui */
	map<const char*, _uint>* GetMessages() {
		return &m_Message;
	}

private:
	/* 애니메이션 이름 */
	char m_szName[MAX_PATH] = "";

	/* 메세지 이름, 다음애니메이션 */
	map<const char*, _uint> m_Message;

	/* 애니메이션의 속성값들. 반복할지? 끝나고 액션을 취할지? */
	_bool						m_bLoop = true;
	_bool						m_bHasExitTime = false;
	
	/* 애니메이션과 애니메이션 사이의 보간 시간 */
	_float						m_fBlendTime = 0.25f;

	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;
	_bool						m_bBlending = false;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;

private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

public:
	static CAnimation* Create(Animation* pAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END