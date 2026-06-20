#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define				TERRIANX       129
#define				TERRIANZ	   129

#define				NULL_TRUE(trueType)    ((trueType)==nullptr)
#define				NULL_FALSE(falseType)  ((falseType)!=nullptr)

#define			SHARED_THIS(_type)	static_pointer_cast<_type>(shared_from_this())
#define			BONE_MATRIX				512
#define			ETOI(_enum)		static_cast<int32_t>(_enum)
#define			ETOUI(_enum)	static_cast<uint32_t>(_enum)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			NS_END						}

#define			NS_USING(NAMESPACE)	using namespace NAMESPACE;


#define WORLD_EVENT_LIST \
X(DOOR)					 \
X(GENERATOR)			 \
X(BOSS_SPAWN)		     \
X(BATTERY)			     \
X(ROLLUP_DOOR)		     \
X(BOSS_DEAD)		     \
X(BOSS_TP)			     \
X(BOSS_LIGHT_FLICK)	     \
X(BOSS_LIGHT_OFF)		 \
X(BOSS_LIGHT_ON)		 \
X(BOSS_EVENT1)			 \
X(BOSS_EVENT2)			 \
X(PLAYER_DEAD)			\
X(BOSS_EVENT3)			\
X(END)

#define PARTICLE_LIST \
X(SPARK)			  \
X(FOG)				  \
X(FOG_SMALL)		  \
X(FOG_CONTROLLER)	  \
X(END)

#define CELL_LIST \
X(NONE)			  \
X(FIRST)		  \
X(SECOND)		  \
X(BOSSTP)		  \
X(ELEVATOR)		  \
X(ARROW)		  \
X(BOSS_EVENT1)	  \
X(BOSS_EVENT2)	  \
X(PLAYER_DEAD)	  \
X(END)

#define LIGHT_STATE_LIST \
X(NONE)					 \
X(LIGHT_BLINK1)			\
X(LIGHT_BLINK2)			\
X(LIGHT_BLINK3)			\
X(LIGHT_BLINK4)			\
X(LIGHT_WORLD)			\
X(LIGHT_OFF)			\
X(LIGHT_ON)				\
X(LIGHT_SLOWOFF)		\
X(LIGHT_SLOWON)			\
X(LIGHT_DEFAULT)



#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


#define NULL_CHECK( _ptr)	\
		{if( _ptr == 0){ return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
		{if( _ptr == 0){return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);return _return;}}

#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK);  return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); return _return;}



#define NO_COPY(CLASSNAME)											\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)								\
			NO_COPY(CLASSNAME)										\
		public:														\
			static CLASSNAME& Get(void) {					\
			static CLASSNAME Instance;								\
			return Instance;										\
		}
				
	
	


//사운드 매니저
#define DOOR_SOUND L"SW_Door_Metal_Rotating_01.wav"
#define ROLLUPDOOR_SOUND L"SW_DormGate_OpenClose_Small.wav"
#define BATTERYCASE_SOUND L"SW_Battery_PowerOn_01.wav"
#define GENERATOR_POWERON_SOUND L"SW_Generator_PowerOn.wav"
#define GENERATOR_POWERON_LOOP L"SW_Generator_IdleHum_LOOP.wav"
#define GENERATOR_BREAK L"SW_School_GeneratorSmash.wav"
#define TEACHER_BGM_SOUND2 L"SW_Get_Out_Music_01_cut_54.wav"
#define LEVER_SOUND L"SW_Lever_Pull_01.wav"
#define POLE_SOUND L"SW_Conduit_ElectricPole_Charged.wav"
#define ELECTRIC_PANNEL_SOUND L"SW_GrabTurret_Shot.wav"
#define ELECTRIC_HAND		 L"SW_Conduit_HandGrab_Coiling.wav"
#define ELEVATOR_LOOP		L"SW_UtilityElevator_Moving_LOOP.wav"
#define ELEVATOR_FINISH		L"SW_UtilityElevator_Stopped.wav"
#define SOUND_BUTTON		L"SW_Interactable_Button.wav"

#define IS_PLAYSOUND(SoundName,ChannelId,Volume)\
if(!CGameInstance::Get().IsPlaying(ChannelId))\
{\
CGameInstance::Get().Play_Sound_Once(SoundName,ChannelId,Volume);\
}

#define PLAY_SOUND(SoundName,ChannelId,Volume)\
 CGameInstance::Get().Play_Sound_Once(SoundName,ChannelId,Volume)

#define STOP_SOUND(ChannelID) CGameInstance::Get().Stop_Sound(ChannelID)

#define VOLCTL(ChannelID,VOLUME)\
CGameInstance::Get().Set_ChannelGroupVolume(ChannelID,VOLUME)

#define GET_VOLUME(ChannelID)\
CGameInstance::Get().Get_ChannelVolume(ChannelID)

#define SOUND_SPEED(ChannelID,Speed)\
CGameInstance::Get().Sound_Speed(ChannelID, Speed)
#endif // Engine_Macro_h__
