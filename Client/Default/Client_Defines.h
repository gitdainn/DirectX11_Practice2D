#pragma once

/* 클라이언트에서 자주사용하는 변수나 정의를 담는다. */
#include "stdafx.h"
#include "Time.h"

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum class SPRITE_DIRECTION { LEFT, RIGHT, DIRECTION_END };

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
		DASH, DEFAULT_ATK, ATK1, ATK2, JUMP_ATK, JUMP,
		FALL, SKILL1, SKILL2, DAMAGED, MOTION_END
	};

	enum class SKUL_RANK
	{
		NORMAL, RARE, UNIQUE, LEGENDARY
	};

	enum class SKUL_TYPE
	{
		BALANCE, POWER, SPEED
	};

	enum class SKILL_TYPE
	{
		MAGIC, PHYISIC, BALANCE
	};

	struct SKUL_EXCEL
	{
		const wchar_t* pName;
		const wchar_t* pSkill[4];

		SKUL_RANK eRank;
		SKUL_TYPE eType;

		int	iMagicAttackIncrease;
		int iPhysicalAttackIncrease;
		int iDefense;

		int iBone;

		const wchar_t* pExplanation;
	};

	struct SKILL_EXCEL
	{
		const wchar_t* pName;

		SKILL_TYPE eType;

		int	iDamage[3];

		double CoolDown;
		double DelayTime;
		double LifeTime;

		const wchar_t* pStatusChange;
		const wchar_t* pExplanation;
	};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;

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
#define	TAG_COLL_DEFAULTATK			TEXT("Com_DefaultAtk")
#define	TAG_FEATURE				TEXT("Com_Feature")
#define	TAG_ANIMKEYFRAME		TEXT("Com_AnimKeyframe")

#define TAG_COLLATK				TEXT("Com_CollAtk")
#define TAG_COLLBODY			TEXT("Com_CollBody")
#define TAG_COLLFOOT			TEXT("Com_CollFoot")
#define TAG_COLLHEAD			TEXT("Com_CollHead")
#pragma endregion

#pragma region LAYER_TAG
#define LAYER_DEFAULT		TEXT("Layer_Default")
#define LAYER_BACKGROUND		TEXT("Layer_Background")
#define LAYER_PLAYER			TEXT("Layer_Player")
#define LAYER_ENEMY				TEXT("Layer_Enemy")
#define LAYER_UI				TEXT("Layer_UI")
#define LAYER_SKILL				TEXT("Layer_Skill")
#pragma endregion

#pragma region DINPUT
#define MOUSE_LB		CDInput_Manager::MOUSEKEYSTATE::DIMK_LB
#define MOUSE_RB		CDInput_Manager::MOUSEKEYSTATE::DIMK_RB
#pragma endregion