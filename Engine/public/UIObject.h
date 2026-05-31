#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{

public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fX, fY, fSizeX, fSizeY;
 }UIOBJECT_DESC;
protected:
	CUIObject(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CUIObject();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float		m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4		m_ViewMatrix{}, m_ProjMatrix{};
protected:
	void	Update_Transfrom();
};

NS_END