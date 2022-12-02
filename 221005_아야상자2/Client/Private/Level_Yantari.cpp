#include "stdafx.h"
#include "..\Public\Level_Yantari.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "GameMgr.h"
#include "UI_Manager.h"
#include "Level_Loading.h"
CLevel_Yantari::CLevel_Yantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Yantari::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_GameObject(L"Prototype_GameObject_Yantari", L"Layer_Yantari")))
		return E_FAIL;
	
	Load();
	for (auto& iter : m_CreateObj)
	{
		if (FAILED(Ready_Layer_GameObject(iter.pPrototypeTag, iter.pLayerTag, &iter)))
			return E_FAIL;
	}

	CUI_Manager::Get_Instance()->Initialize();
	return S_OK;
}

void CLevel_Yantari::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CUI_Manager::Get_Instance()->Tick(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLevel_Yantari::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CUI_Manager::Get_Instance()->Render();
	SetWindowText(g_hWnd, TEXT("LEVEL_YANTARI"));

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(15.0f, 5.0f, 15.0f, 1.f);
	//LightDesc.fRange = 10.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 0.f, 0.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(.1f, .1f, .1f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Free"), LEVEL_YANTARI, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_YANTARI, pLayerTag)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Terrain"), LEVEL_YANTARI, pLayerTag)))
		return E_FAIL;

	struct  Temp
	{
		_tchar pPrototypeTag[MAX_PATH] = L"Prototype_Component_Model_SkyDomeLevel6";
		_uint iNumLevel = LEVEL_END;
	};
	Temp tTemp;
	tTemp.iNumLevel = LEVEL_YANTARI;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Sky"), LEVEL_YANTARI, pLayerTag, &tTemp)))
		return E_FAIL;


	/*if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ForkLift"), LEVEL_GAMEPLAY, pLayerTag)))
	return E_FAIL;*/

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RasSamrah"), LEVEL_YANTARI, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/*for (_uint i = 0; i < 1; ++i)
	{
	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, pLayerTag)))
	return E_FAIL;

	}

	*/
	Safe_Release(pGameInstance);



	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//for (_uint i = 0; i < 10; ++i)
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY, pLayerTag)))
	//		return E_FAIL;

	//}


	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_ForkLift(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ForkLift"), LEVEL_GAMEPLAY, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Ready_Layer_GameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, void* pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, LEVEL_YANTARI, pLayerTag, pArg)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Yantari::Load()
{
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/MapStaticInfoYantari.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	//프로토타입
	//레이어
	//모델
	//이름(char)
	//레벨
	//매트릭스
	while (true)
	{
		//프로토타입 로드
		CREATE_INFO tObjInfo;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pPrototypeTag, dwStrByte, &dwByte, nullptr);

		//레이어 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pLayerTag, dwStrByte, &dwByte, nullptr);

		//모델 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pModelTag, dwStrByte, &dwByte, nullptr);

		//이름 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.szName, dwStrByte, &dwByte, nullptr);

		//레벨 로드
		ReadFile(hFile, &tObjInfo.iNumLevel, sizeof(_uint), &dwByte, nullptr);

		//매트릭스 로드
		ReadFile(hFile, &tObjInfo.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
		m_CreateObj.push_back(tObjInfo);
	}

	CloseHandle(hFile);
	return S_OK;
}

CLevel_Yantari * CLevel_Yantari::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Yantari*		pInstance = new CLevel_Yantari(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Yantari"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Yantari::Free()
{
	__super::Free();
	CUI_Manager::Get_Instance()->Destroy_Instance();
	CGameMgr::Get_Instance()->Destroy_Instance();
}