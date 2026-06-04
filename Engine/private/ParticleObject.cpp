#include "ParticleObject.h"
#include "GameObject.h"
CParticleObject::CParticleObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CParticleObject::CParticleObject(const CParticleObject& Prototype) : CGameObject(Prototype)
{
}

CParticleObject::~CParticleObject()
{
}

