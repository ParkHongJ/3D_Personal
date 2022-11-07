#pragma once
#include "Client_Defines.h"
#include "Base.h"
BEGIN(Client)
class CGameMgr final : public CBase
{
	DECLARE_SINGLETON(CGameMgr)
public:
	CGameMgr();
	virtual ~CGameMgr() = default;

public:
	void RegisterCamera(class CCamera_Free* pCamera);
	void RegisterPlayer(class CPlayer* pPlayer);

	_float Tick(_float fTimeDelta);
	//얼마나줄일지, 몇초동안
	void SetTimeScale(_float fTimeScale, _float fDuration);
	void Shake(_float fShakeTime = 0.15f, _float fShakeStrength = 0.25f);

	_float GetTimeDelta() { return m_fOldTimeDelta; }
private:
	class CPlayer* m_pPlayer = nullptr;
	class CCamera_Free* m_pCamera = nullptr;

	//For Time
	_float m_fDuration = 0.0f;
	_float m_fTimeScale = 0.0f;
	_float m_fCurrentTime = 0.0f;
	_bool  m_bTimeScale = false;

	_float m_fOldTimeDelta = 0.0f;
public:
	virtual void Free() override;
};
END
