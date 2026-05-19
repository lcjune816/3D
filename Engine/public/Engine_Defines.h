#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dx11Effect.h>
#include "d3dcompiler.h"

#include "DirectXTK-main/DDSTextureLoader.h"

#include "DirectXCollision.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "implot3d_internal.h"
#include "implot3d.h"
#include "ImGuizmo.h"

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>

#include <fstream>
#include <iostream>
#include <filesystem>

#include <string>

#include <unordered_map>
#include <ctime>
#include <memory>
#include <wrl/client.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "json/json.hpp"
using namespace Microsoft::WRL;

using namespace DirectX;
using namespace std;

using json = nlohmann::json;
#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"


#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

using namespace Engine;


