#include "stdafx.h"
#include "..\Public\UI_Manager.h"
#include "ProgressBar.h"
#include "GameInstance.h"
IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}
void CUI_Manager::Initialize(const _tchar * pPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*>* pLayers = pGameInstance->GetLayers(L"Canvas", LEVEL_GAMEPLAY);

	for (auto& iter : *pLayers)
	{
		m_ProgressBar.insert({ (_char*)iter->GetName(), (CProgressBar*)iter });
		Safe_AddRef(iter);
	}
	RELEASE_INSTANCE(CGameInstance);

}
void CUI_Manager::SetValue(const _char * pUITag, _float fValue)
{

}
void CUI_Manager::Tick(_float fTimeDelta)
{

}

void CUI_Manager::Render()
{
}

void CUI_Manager::Free()
{
	for (auto& Pair : m_ProgressBar)
	{
		Safe_Release(Pair.second);
	}
	m_ProgressBar.clear();
}
