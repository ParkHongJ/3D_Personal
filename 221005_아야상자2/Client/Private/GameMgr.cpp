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

_float CGameMgr::Tick(_float fTimeDelta)
{
	_float fReturnTimeDelta = fTimeDelta;
	m_fCurrentTime += fTimeDelta;
	m_fOldTimeDelta = fTimeDelta;
	//최대시간을 넘지 않았다면
	if (m_fCurrentTime < m_fDuration && m_bTimeScale)
	{
		fReturnTimeDelta = fReturnTimeDelta * m_fTimeScale;
	}
	else
	{
		m_bTimeScale = false;
		m_fCurrentTime = 0.0f;
	}
	return fReturnTimeDelta;
}

void CGameMgr::SetTimeScale(_float fTimeScale, _float fDuration)
{
	if (!m_bTimeScale)
	{
		m_fTimeScale = fTimeScale;
		m_fDuration = fDuration;
		m_bTimeScale = true;
	}
}

void CGameMgr::Shake(_float fShakeTime, _float fShakeStrength)
{
	m_pCamera->ShakeStart(fShakeTime, fShakeStrength);
}

void CGameMgr::Free()
{
	Safe_Release(m_pCamera);
	Safe_Release(m_pPlayer);
}
