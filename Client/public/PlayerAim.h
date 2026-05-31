#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CRect;
class CShader;
NS_END

NS_BEGIN(Client)
class CPlayerAim : public CUIObject
{
public:
	typedef struct tagPlayerAim : public CUIObject::UIOBJECT_DESC
	{
		uint32_t iData;
	}PLAYERAIM_DESC;
private:
	CPlayerAim(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayerAim(const CPlayerAim& Prototype);
public:
	virtual ~CPlayerAim();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	shared_ptr<CRect>		m_pBuffer = { nullptr };
	shared_ptr<CShader>		m_pShader = { nullptr };
	
	uint32_t				m_iTextureID = {};
private:
	HRESULT			Ready_Component();
public:
	static unique_ptr<CPlayerAim> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg)override;

};


NS_END