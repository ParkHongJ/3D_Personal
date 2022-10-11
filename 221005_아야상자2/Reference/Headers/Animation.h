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
	/* �ִϸ��̼� �̸� */
	char m_szName[MAX_PATH] = "";

	/* �޼��� �̸�, �����ִϸ��̼� */
	map<const char*, _uint> m_Message;

	/* �ִϸ��̼��� �Ӽ�����. �ݺ�����? ������ �׼��� ������? */
	_bool						m_bLoop = true;
	_bool						m_bHasExitTime = false;
	
	/* �ִϸ��̼ǰ� �ִϸ��̼� ������ ���� �ð� */
	_float						m_fBlendTime = 0.25f;

	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;
	_bool						m_bBlending = false;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;

private: /* ������ �ִϸ��̼� ���� ���� ������. */
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;

public:
	static CAnimation* Create(Animation* pAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;
};

END