#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <random>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <directxtk/SimpleMath.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include "physx/PxPhysicsAPI.h"
#include <directxcollision.h>

#include <string>
#include <clocale>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include <fstream>
#include <filesystem>

#include <io.h>
#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodL_vc.lib")

using namespace DirectX;
using namespace physx;
#define MAX_NUM_ACTOR_SHAPES 128
using namespace std;
using namespace std::filesystem;
using namespace FMOD;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif // _DEBUG
