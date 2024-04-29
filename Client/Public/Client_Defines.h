#pragma once

/* Ŭ���̾�Ʈ���� ���ֻ���ϴ� ������ ���Ǹ� ��´�. */
#include "Time.h"

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL {LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END};

	typedef enum class eVTXTEX_PASS
	{
		Default,
		UV_Anim
	}VTXTEX_PASS;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

#pragma region LAYER
#define	PLAYER_LAYER			TEXT("Layer_Player")
#pragma endregion

#pragma region COMPONENT_TAG
#define	TAG_TRANSFORM			TEXT("Com_Transform")
#define	TAG_RENDERER			TEXT("Com_Renderer")
#define	TAG_SHADER				TEXT("Com_Shader")
#define	TAG_MODEL				TEXT("Com_Model")
#define	TAG_BUFFER				TEXT("Com_VIBuffer")
#define	TAG_TEXTURE				TEXT("Com_Texture")
#define	TAG_CALCULATOR			TEXT("Com_Calculator")
#define	TAG_NAVIGATION			TEXT("Com_Navigation")
#define	TAG_COLL_OBB			TEXT("Com_OBB")
#define	TAG_COLL_SPHERE			TEXT("Com_Sphere")
#define	TAG_COLL_AABB			TEXT("Com_AABB")
#define	TAG_FEATURE				TEXT("Com_Feature")
#define	TAG_ANIMKEYFRAME		TEXT("Com_AnimKeyframe")

#define TAG_COLLATK				TEXT("Com_CollAtk")
#define TAG_COLLBODY			TEXT("Com_CollBody")
#define TAG_COLLFOOT			TEXT("Com_CollFoot")
#define TAG_COLLHEAD			TEXT("Com_CollHead")
#pragma endregion

#pragma region LAYER_TAG
#define LAYER_BACKGROUND		TEXT("Layer_Background")
#define LAYER_PLAYER			TEXT("Layer_Player")
#define LAYER_UI				TEXT("Layer_UI")
#pragma endregion

#pragma region DINPUT
#define MOUSE_LB		CDInput_Manager::MOUSEKEYSTATE::DIMK_LB
#define MOUSE_RB		CDInput_Manager::MOUSEKEYSTATE::DIMK_RB
#pragma endregion