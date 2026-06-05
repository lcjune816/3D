#ifndef Engine_Enum_h__
#define Engine_Enum_h__
#include "Engine_Macro.h"
namespace Engine
{
	enum class WINMODE { FULL, WIN };
	
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
	enum class STATE { RIGHT, UP, LOOK, POS, END };
	enum class MESH_TYPE { ANIME, NONANIME, TRIGGER ,DECAL,NAVI,END };
	enum class D3DTS   {VIEW, PROJ, END};
	enum class FSM     {IDLE, MOVE, JUMP,CROUCH, HAND,PULL_HAND, SPAWN, END};
	enum class FLAGVALUE { ENABLE, DISABLE, TOGGLE,RESET, END };
	enum class EPOINT		{A,B,C,END};
	enum class LINE  {AB, BC, CA ,END};
	enum class COLLISION { NORMAL, TRIGGER, END };
	enum class DIMKEYINPUT{F5,F6,F7,F8,F9, G,TAB,Z,A,S, X,C,END};
	enum class DIMK{LBUTTON, RBUTTON, WHEEL, END};
	enum class DIMM {X,Y,WHEEL,END};
	enum class MATERIAL : uint32_t {NORMAL,GLASS};
	enum class ENGINE_MOUSE{ HOLD, A_CLICK, B_CLICK };

	enum class PARTICLE_SHAPE { POINT, END };
	#define X(name) name,
	enum class PARTICLE { PARTICLE_LIST };
	enum class WORLD_EVENT { WORLD_EVENT_LIST };
	enum class CELL_EVENT { CELL_LIST };
	#undef X
	
	enum class SPAWN {PLAYER,MONSTER, END};
	//// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	//enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	
	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	
	//enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

	//enum MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	
	enum class GUI_OBJECT{CHANGER,ANIMATION,END};

}
#endif // Engine_Enum_h__
