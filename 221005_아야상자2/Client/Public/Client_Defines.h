#pragma once
#define MoveForward DIK_UP 
#define MoveBack DIK_DOWN 
#define MoveLeft DIK_LEFT 
#define MoveRight DIK_RIGHT 
namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	typedef struct CreateObjInfo
	{
		//생성할때 필요한 자료.
		//이름
		//프로토타입 태그
		//레이어 태그
		//레벨
		//모델명
		char szName[MAX_PATH] = "";
		wchar_t pPrototypeTag[MAX_PATH] = L"";
		wchar_t pLayerTag[MAX_PATH] = L"";
		unsigned int iNumLevel = LEVEL_END;
		wchar_t pModelTag[MAX_PATH] = L"";
	}CREATE_INFO;
	
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
