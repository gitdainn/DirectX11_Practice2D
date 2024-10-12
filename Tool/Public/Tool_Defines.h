#pragma once

/* 클라이언트에서 자주사용하는 변수나 정의를 담는다. */
#include "stdafx.h"
#include "Time.h"

namespace Tool
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL {LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_END};

	enum class SPRITE_DIRECTION { LEFT, RIGHT , DIRECTION_END };

	typedef enum class eVTXTEX_PASS
	{
		Default,
		UV_Anim,
		FlipUV_Anim
	}VTXTEX_PASS;

	enum class CONTROL_KEY 
	{ 
		LEFT, RIGHT, FALL, 
		JUMP, DASH, MENU,
		INTERACTION, ATTACK, SKILL1,
		SKILL2, ESSENCE, ROTATION
	};

	enum class STATE_TYPE 
	{ 
		IDLE, WALK,
		DASH, ATK1, ATK2, JUMP_ATK, JUMP,
		FALL, SKILL1, SKILL2, MOTION_END 
	};

	typedef enum LAYER
	{
		DEFAULT = (1 << 0), // 0000 0001
		BACKGROUND = (1 << 1), // 0000 0010
		PLAYER = (1 << 2), // 0000 0100
		ENEMY = (1 << 3), // 0000 1000
		PLAYER_ATK = (1 << 4),
		ENEMY_ATK = (1 << 5),
		CAMERA = (1 << 6),
		EFFECT = (1 << 7),
		ITEM = (1 << 8),
		UI = (1 << 9), // 0001 0000
		UI_MAINSKUL_STATE = (1 << 10), // 0001 0000
		UI_SUBSKUL_STATE = (1 << 11), // 0001 0000
	}LAYER_BITSET;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tool;

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

#pragma region DINPUT
#define MOUSE_LB		CDInput_Manager::MOUSEKEYSTATE::DIMK_LB
#define MOUSE_RB		CDInput_Manager::MOUSEKEYSTATE::DIMK_RB
#pragma endregion