#pragma once

#include <Windows.h>
#include <memory>
#include <process.h>
#include "Engine_Defines.h"

namespace Client
{
#define    SPTCAST(Src)  static_pointer_cast<Src>
#define    DPTCAST(Src)  dynamic_pointner_cast<Src>


	static const unsigned int	   g_iWinSizeX = { 1280 };
	static const unsigned int	   g_iWinSizeY = { 720 };
	
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, GASZONE, END};

	enum class MOVE { IDLE, LEFT, RIGHT, FORWARD, BACKWARD, RUN,JUMP, END };
	
	enum class PLAYER_HAND{WALL,TRIGGER,FORCE, END};

	enum class  PATHNAME { SHADER, BUFFER, TEXTURE, END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;


using namespace std;
using namespace Client;