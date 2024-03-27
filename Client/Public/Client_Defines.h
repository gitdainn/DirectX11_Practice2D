#pragma once

/* 클라이언트에서 자주사용하는 변수나 정의를 담는다. */
#include "Time.h"

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL {LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
