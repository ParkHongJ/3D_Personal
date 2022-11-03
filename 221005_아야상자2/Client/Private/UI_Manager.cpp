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
	Load();

	

}
void CUI_Manager::SetValue(const char * pUITag, _float fValue)
{
	auto	iter = find_if(m_ProgressBar.begin(), m_ProgressBar.end(), CTag_FinderC(pUITag));

	if (iter == m_ProgressBar.end())
		return;

	iter->second->SetValue(fValue);
}
void CUI_Manager::Tick(_float fTimeDelta)
{

}

void CUI_Manager::Render()
{
}

HRESULT CUI_Manager::Load()
{
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/GAMEPLAY_UI.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	//프로토타입
	//Texture
	//이름(char)
	//레벨
	//매트릭스
	//패스
	//알파
	while (true)
	{
		//프로토타입 로드
		CREATE_UI_INFO tObjInfo;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pPrototypeTag, dwStrByte, &dwByte, nullptr);

		//모델 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pTextureTag, dwStrByte, &dwByte, nullptr);

		//이름 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.szName, dwStrByte, &dwByte, nullptr);

		//레벨 로드
		ReadFile(hFile, &tObjInfo.iNumLevel, sizeof(_uint), &dwByte, nullptr);

		//매트릭스 로드
		ReadFile(hFile, &tObjInfo.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		//패스 로드
		ReadFile(hFile, &tObjInfo.iPass, sizeof(_uint), &dwByte, nullptr);
		//알파 로드
		ReadFile(hFile, &tObjInfo.fAlpha, sizeof(_float), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(tObjInfo.pPrototypeTag, tObjInfo.iNumLevel, L"Canvas", &tObjInfo);
		RELEASE_INSTANCE(CGameInstance);
	}

	CloseHandle(hFile);

	//UI오브젝트들 등록
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*>* pLayers = pGameInstance->GetLayers(L"Canvas", LEVEL_GAMEPLAY);

	for (auto& iter : *pLayers)
	{
		m_ProgressBar.insert({ iter->GetName(), (CProgressBar*)iter });
		Safe_AddRef(iter);
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

void CUI_Manager::Free()
{
	for (auto& Pair : m_ProgressBar)
	{
		Safe_Release(Pair.second);
	}
	m_ProgressBar.clear();
}
