#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>

#include <DirectXMath.h>
#include <Process.h>

using namespace std;
using namespace DirectX;

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;