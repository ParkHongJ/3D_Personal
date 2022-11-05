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

	void Shake();
private:
	class CPlayer* m_pPlayer = nullptr;
	class CCamera_Free* m_pCamera = nullptr;
public:
	virtual void Free() override;
};
END
