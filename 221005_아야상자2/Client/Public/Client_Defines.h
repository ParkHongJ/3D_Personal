#pragma once
#define MoveForward DIK_UP 
#define MoveBack DIK_DOWN 
#define MoveLeft DIK_LEFT 
#define MoveRight DIK_RIGHT 
#define LockON DIK_NUMPAD9
namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_YANTARI, LEVEL_END };

	
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
using namespace Client;
