#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CParticleObject abstract : public CGameObject
{
public:
	typedef struct strparticledesc : public CGameObject::GAMEOBJECT_DESC
	{
		WORLD_EVENT	eParticleType;
	}PARTICLEOBJECT_DESC;
protected:
	CParticleObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CParticleObject(const CParticleObject& Prototype);
public:
	virtual ~CParticleObject();

public:
	_bool			EndCycle() { return m_bEndCycle; }
	_bool			CompareParticleType(WORLD_EVENT eType)
	{
		if (m_eParticleType == eType)
			return true;

		return false;
	}
protected:
	WORLD_EVENT								m_eParticleType{ WORLD_EVENT::END };
	_bool									m_bEndCycle{ false };
public:
	virtual		shared_ptr<CPrototype> Clone(void* pArg) PURE;

};


NS_END
