#pragma once
#include "UIObject.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
class CRect;
NS_END

NS_BEGIN(Client)
class CUILoadingScreen : public CUIObject
{
public:
	CUILoadingScreen(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUILoadingScreen(const CUIObject& Prototype );
public:
	~CUILoadingScreen();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;


private:
	int32_t							m_iTextureID = { -1 };

	shared_ptr<Engine::CShader>		m_pShaderCom = { nullptr };
	shared_ptr<Engine::CRect>		m_pVIBufferCom = { nullptr };
public:
	static		unique_ptr<CUILoadingScreen>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype>	Clone(void* pArg) override;
};

NS_END

