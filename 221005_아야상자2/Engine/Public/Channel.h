#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

public:
	HRESULT Initialize(NodeAnim*	pChannel);
	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);

	_uint Blending_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);
	void SetOldMatrix(_fmatrix OldMatrix);
	_matrix Get_OldMatrix() { return XMLoadFloat4x4(&m_OldMatrix); }

private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;
	_float4x4						m_OldMatrix;


public:
	static CChannel* Create(NodeAnim*	pChannel);
	virtual void Free() override;
};

END