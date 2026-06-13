#pragma once

#include "Engine_Defines.h"
#include <../Fmodel/fmod.hpp>
#include <../Fmodel/fmod.h>
#include <../Fmodel/fmod_errors.h>

using namespace FMOD;

NS_BEGIN(Engine)
class ENGINE_DLL SoundManager final {
	DECLARE_SINGLETON(SoundManager)
private:
	SoundManager();
public:
	~SoundManager();

public:
	HRESULT		Ready_SoundManager();
	VOID		Update_SoundManager();

	HRESULT	    Play_Sound_Once(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume = 0.5f);
	HRESULT		Play_Sound(CONST TCHAR* _FilePath, CHANNELID _SoundChannel,_float Volume = 0.5f,_bool ChanelMode = TRUE);
	HRESULT		Stop_Sound(CHANNELID _SoundChannel);
	HRESULT		Stop_AllSound();

	BOOL		IsPlaying(CHANNELID _SoundChannel);
	VOID		Set_ChannelVolume(CHANNELID _CID, FLOAT Volume) { 
		SoundChannel[(LONG)_CID]->setVolume(Volume);
		int a = 0;
		return;
	}
	VOID		Set_ChannelGroupVolume(CHANNELID _CID, FLOAT Volume) { ChannelGroup[(LONG)_CID]->setVolume(Volume); }
	FLOAT		Get_ChannelVolume(CHANNELID _CID) {
		FLOAT F = 0.f;
		ChannelGroup[(LONG)_CID]->getVolume(&F);
		return F;
	}
private:
	System*	SoundSystem; 

	map<CONST _wstring, Sound*>	SoundMap;
	
	ChannelGroup*	ChannelGroup[(LONG)CHANNELID::SOUND_END];
	Channel*		SoundChannel[(LONG)CHANNELID::SOUND_END];

public:
	static			unique_ptr<SoundManager>Create();
private:
	virtual VOID	Free();
};

NS_END