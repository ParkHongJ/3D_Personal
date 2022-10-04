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
	/* �� �ִϸ��̼��� �����ϱ����� ���Ǵ� ���� ����. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* �ִϸ��̼� ����ϴµ� �ɸ��� ��ü�ð�. */
	_float						m_fDuration = 0.f;

	/* �ִϸ��̼��� �ʴ� ��� �ӵ�. */
	_float						m_fTickPerSecond = 0.f;
	_float						m_fPlayTime = 0.f;

private: /* ������ �ִϸ��̼� ���� ���� ������. */
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