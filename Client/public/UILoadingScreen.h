#pragma once
#include "UIObject.h"
#include "Client_Defines.h"
#include "Observer.h"
NS_BEGIN(Engine)
class CShader;
class CRect;
NS_END


enum class SCREEN{FADEIN = 1, FADEOUT = 2, LOADING = 3, BLACK, DEADSCREEN, END};
NS_BEGIN(Client)
class CUILoadingScreen : public CUIObject, public CObserver
{
public:
	typedef struct strLoadingScreen : CUIObject::UIOBJECT_DESC
	{
		SCREEN		eType;
	}LOADING_SCREEN_DESC;
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

public:
	virtual void OnNotify(const EVENT& eEvent) override;
public:
	void						Change_Screen(SCREEN eType) {
		m_eType = eType; m_bStop = false;
		m_fTimeRange = 0.f;
		if (m_eType == SCREEN::END)
		{

			m_bEnd = true;
			m_eType = SCREEN::LOADING;
		}
	}
private:
	void						FadeIn(const _float& fTimeDelta);
	void						FadeOut(const _float& fTimeDelta);
	void						Loading(const _float& fTimeDelta);
private:
	_bool							m_bStop = false;

	int32_t							m_iTextureID[ETOUI(TEXTUREID::END)] = {-1};
	TEXTUREID						m_eTextureType = {};
	SCREEN							m_eType = {};

	_bool							m_bEnd{ false };
	_float							m_fLoadingTick{}, m_fTimeTick = {}, m_fTimeRange = {};
	_float4							m_fTexCoord = {};
	shared_ptr<Engine::CShader>		m_pShaderCom = { nullptr };
	shared_ptr<Engine::CRect>		m_pVIBufferCom = { nullptr };
public:
	static		unique_ptr<CUILoadingScreen>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype>	Clone(void* pArg) override;
};

NS_END

