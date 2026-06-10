#pragma once
#include "GameObject.h"


NS_BEGIN(Engine)
class ENGINE_DLL CLightEffect : public CGameObject
{
public:
	typedef struct strparticledesc : public CGameObject::GAMEOBJECT_DESC
	{
		WORLD_EVENT	eWorldEventType;
		LIGHT_STATE eLocalEventType;
		LIGHT_HANDLE eHandle;

	}LIGHTEFFECT_DESC;
protected:
	CLightEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLightEffect(const CLightEffect& Prototype);
public:
	virtual ~CLightEffect();

public:

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);
	virtual HRESULT	Render();

protected:
	WORLD_EVENT								m_eWorldEventType{ WORLD_EVENT::END };
	LIGHT_STATE								m_eLocalEventType{ LIGHT_STATE::NONE};
	LIGHT_HANDLE							m_eHandle = {};
public:
	virtual		shared_ptr<CPrototype> Clone(void* pArg) PURE;

};

NS_END

