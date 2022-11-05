#include "stdafx.h"
#include "..\Public\GameMgr.h"
#include "Camera_Free.h"
#include "Player.h"
#include "GameObject.h"
#include "Camera_Free.h"
IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
{
}
void CGameMgr::RegisterCamera(CCamera_Free * pCamera)
{
	pCamera->Set_Player((CTransform*)m_pPlayer->Get_ComponentPtr(L"Com_Transform"));
	m_pCamera = pCamera;
	Safe_AddRef(m_pCamera);

	m_pPlayer->Set_Camera(pCamera);
}

void CGameMgr::RegisterPlayer(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
	Safe_AddRef(m_pPlayer);
}

void CGameMgr::Free()
{
	Safe_Release(m_pCamera);
	Safe_Release(m_pPlayer);
}
