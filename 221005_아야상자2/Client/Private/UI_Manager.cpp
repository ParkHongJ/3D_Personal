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

	//���� ����Ʈ 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	//������Ÿ��
	//Texture
	//�̸�(char)
	//����
	//��Ʈ����
	//�н�
	//����
	while (true)
	{
		//������Ÿ�� �ε�
		CREATE_UI_INFO tObjInfo;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pPrototypeTag, dwStrByte, &dwByte, nullptr);

		//�� �ε�
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pTextureTag, dwStrByte, &dwByte, nullptr);

		//�̸� �ε�
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.szName, dwStrByte, &dwByte, nullptr);

		//���� �ε�
		ReadFile(hFile, &tObjInfo.iNumLevel, sizeof(_uint), &dwByte, nullptr);

		//��Ʈ���� �ε�
		ReadFile(hFile, &tObjInfo.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		//�н� �ε�
		ReadFile(hFile, &tObjInfo.iPass, sizeof(_uint), &dwByte, nullptr);
		//���� �ε�
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

	//UI������Ʈ�� ���
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
