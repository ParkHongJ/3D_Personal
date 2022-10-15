#include "stdafx.h"
#include "..\Public\GameMgr.h"
#include "Camera_Free.h"
#include "Player.h"
#include "GameObject.h"
IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
{
}
void CGameMgr::RegisterCamera(CCamera_Free * pCamera)
{
	pCamera->Set_Player((CTransform*)m_pPlayer->Get_ComponentPtr(L"Com_Transform"));
	m_pPlayer->Set_Camera(pCamera);
	m_pPlayer = nullptr;
}

void CGameMgr::RegisterPlayer(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
}

void CGameMgr::Free()
{

}
