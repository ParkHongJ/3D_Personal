#include "..\Public\Key_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CKey_Manager)

CKey_Manager::CKey_Manager()
{
}

_bool CKey_Manager::Key_Down(_uchar KeyInput)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!m_bKeyState[KeyInput] && (pGameInstance->Get_DIKState(KeyInput) & 0x80)) {
		m_bKeyState[KeyInput] = true;
		Safe_Release(pGameInstance);
		return true;
	}

	for (_uint i = 0; i < 256; ++i) {
		if (m_bKeyState[i] && !(pGameInstance->Get_DIKState(i) & 0x80)) {
			m_bKeyState[i] = false;
		}
	}

	Safe_Release(pGameInstance);
	return false;
}

_bool CKey_Manager::Mouse_Down(DIMK eMouseKeyID)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (!m_bKeyState[eMouseKeyID] && (pGameInstance->Get_DIMKeyState(eMouseKeyID) & 0x80)) {
		m_bKeyState[eMouseKeyID] = true;
		Safe_Release(pGameInstance);
		return true;
	}

	for (_uint i = 0; i < 256; ++i) {
		if (m_bKeyState[i] && !(pGameInstance->Get_DIMKeyState((DIMK)i) & 0x80)) {
			m_bKeyState[i] = false;
		}
	}

	Safe_Release(pGameInstance);
	return false;
}

_bool CKey_Manager::Key_Pressing(_uchar KeyInput)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKState(KeyInput) & 0x80) {
		m_bKeyState[KeyInput] = true;
		Safe_Release(pGameInstance);
		return true;
	}

	Safe_Release(pGameInstance);
	return false;
}

_bool CKey_Manager::Key_Up(_uchar KeyInput)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (m_bKeyState[KeyInput] && !(pGameInstance->Get_DIKState(KeyInput) & 0x8000))
	{
		m_bKeyState[KeyInput] = !m_bKeyState[KeyInput];
		return true;
	}

	// key 상태 원래 대로 복원
	for (int i = 0; i < 256; ++i)
	{
		if (!m_bKeyState[i] && (pGameInstance->Get_DIKState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
	}

	Safe_Release(pGameInstance);
	return false;
}

void CKey_Manager::Free()
{
}
