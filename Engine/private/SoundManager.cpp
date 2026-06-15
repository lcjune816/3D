#include "SoundManager.h"

SoundManager::SoundManager() {}
SoundManager::~SoundManager() { Free(); }

HRESULT SoundManager::Ready_SoundManager() {
	System_Create(&SoundSystem);
	SoundSystem->init(64, FMOD_INIT_NORMAL, NULL);

	SoundSystem->createChannelGroup("BGM1",		&ChannelGroup[(int)CHANNELID::SOUND_BGM01]);
	SoundSystem->createChannelGroup("BGM2",		&ChannelGroup[(int)CHANNELID::SOUND_BGM02]);
	SoundSystem->createChannelGroup("BGM3",		&ChannelGroup[(int)CHANNELID::SOUND_BGM03]);
	SoundSystem->createChannelGroup("PlayerMove", &ChannelGroup[(int)CHANNELID::PLAYER_MOVE]);
	SoundSystem->createChannelGroup("PlayerEffect", &ChannelGroup[(int)CHANNELID::PLAYER_EFFECT]);
	SoundSystem->createChannelGroup("Effect1",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT01]);
	SoundSystem->createChannelGroup("Effect2",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT02]);
	SoundSystem->createChannelGroup("Effect3",	&ChannelGroup[(int)CHANNELID::SOUND_EFFECT03]);
	SoundSystem->createChannelGroup("Object", &ChannelGroup[(int)CHANNELID::SOUND_OBJECT]);
	SoundSystem->createChannelGroup("Object2", &ChannelGroup[(int)CHANNELID::SOUND_OBJECT2]);
	SoundSystem->createChannelGroup("Object3", &ChannelGroup[(int)CHANNELID::SOUND_OBJECT3]);
	SoundSystem->createChannelGroup("Object4", &ChannelGroup[(int)CHANNELID::SOUND_OBJECT4]);
	SoundSystem->createChannelGroup("Object_WorldEvent", &ChannelGroup[(int)CHANNELID::SOUND_WORLDEVENT]);
	SoundSystem->createChannelGroup("Boss", &ChannelGroup[(int)CHANNELID::SOUND_BOSS]);
	SoundSystem->createChannelGroup("Boss_Effect", &ChannelGroup[(int)CHANNELID::SOUND_BOSS_EFFECT]);

	_wstring path = L"../../Sounds/Loop";
	for (auto& iter : filesystem::recursive_directory_iterator(path))
	{
		if (iter.path().extension() == ".wav" || iter.path().extension() == ".mp3")
		{
			Sound* pSound = nullptr;
			SoundSystem->createStream(iter.path().string().c_str(), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_IGNORETAGS, nullptr, &pSound);
			SoundMap.emplace(iter.path().filename().wstring(), pSound);
		}
	}
	path = L"../../Sounds/NoLoop";
	for (auto& iter : filesystem::recursive_directory_iterator(path))
	{
		if (iter.path().extension() == ".wav" || iter.path().extension() == ".mp3")
		{
			Sound* pSound = nullptr;
			SoundSystem->createSound(iter.path().string().c_str(), FMOD_LOOP_OFF | FMOD_2D | FMOD_IGNORETAGS, nullptr, &pSound);
			SoundMap.emplace(iter.path().filename().wstring(), pSound);
		}
	}
	return S_OK;
}
VOID SoundManager::Update_SoundManager() {
  SoundSystem->update();
}

HRESULT SoundManager::Play_Sound_Once(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume)
{

	Channel* pSound = nullptr;

	auto iter = SoundMap.find(_FilePath);

	if (iter == SoundMap.end()) {
		return E_FAIL;
	}
	else {
		SoundSystem->playSound(iter->second, ChannelGroup[(int)_SoundChannel], FALSE, &pSound);
	}
	ChannelGroup[(LONG)_SoundChannel]->setVolume(Volume);
	return S_OK;


}
HRESULT SoundManager::Play_Sound(CONST TCHAR* _FilePath, CHANNELID _SoundChannel, _float Volume, _bool ChanelMode) {
  Sound* SoundObject = nullptr;

	 Channel* pSound = nullptr;

	 auto iter = SoundMap.find(_FilePath);

	 if (iter == SoundMap.end()) {
		  return E_FAIL;
	 }
	 else {
		  SoundSystem->playSound(iter->second, ChannelGroup[(int)_SoundChannel], FALSE, &pSound);
	 }
	 ChannelGroup[(LONG)_SoundChannel]->setVolume(Volume);
	
	if(ChanelMode)
		SoundChannel[(LONG)_SoundChannel]->setVolume(Volume);
	else ChannelGroup[(LONG)_SoundChannel]->setVolume(Volume);

	return S_OK;
}
HRESULT SoundManager::Stop_Sound(CHANNELID _SoundChannel) {
	ChannelGroup[(LONG)_SoundChannel]->stop();
  return S_OK;
}
HRESULT SoundManager::Stop_AllSound() {
  for (INT ID = 0; ID < (LONG)CHANNELID::SOUND_END; ++ID) {
    SoundChannel[(LONG)ID]->stop();
  }

  for (INT ID = 0; ID < (LONG)CHANNELID::SOUND_END; ++ID)
  {
	  ChannelGroup[(LONG)ID]->stop();
  }
  return S_OK;
}
BOOL SoundManager::IsPlaying(CHANNELID _SoundChannel) {
  bool PlayingCheck = FALSE;

  ChannelGroup[(LONG)_SoundChannel]->isPlaying(&PlayingCheck);
  return PlayingCheck;
}

VOID SoundManager::Free() {
  for (auto& Sound : SoundMap) {
    Sound.second->release();
  }
  for (size_t i = 0; i < ETOUI(CHANNELID::SOUND_END); ++i)
  {
	  ChannelGroup[i]->release();
  }
	 
  SoundMap.clear();
  SoundSystem->release();
  SoundSystem->close();
} 

unique_ptr<SoundManager> SoundManager::Create()
{
	auto pInstance = unique_ptr<SoundManager>(new SoundManager);

	if (FAILED(pInstance->Ready_SoundManager()))
	{
		MSG_BOX("Create Failed SoundManager");
		return nullptr;
	}

	return pInstance;
		
}