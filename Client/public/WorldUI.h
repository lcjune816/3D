#pragma once
#include "UIObject.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
class CRect;
NS_END


enum class NOTICE { SPEECH1, SPEECH2, SPEECH3, SPEECH4, SPEECH5, SPEECH6, SPEECH7, SPEECH8, END };

NS_BEGIN(Client)

class CWorldUI : public CUIObject
{
public:
	typedef struct strworlduidesc: CUIObject::UIOBJECT_DESC
	{
		NOTICE		eType;
	}WORLDUI;
public:
	CWorldUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWorldUI(const CUIObject& Prototype);
public:
	~CWorldUI();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

public:

private:
private:
	_bool							m_bStop = false;

	uint32_t						m_iFade{5};
	int32_t							m_iTextureID[ETOUI(TEXTUREID::END)] = {-1};
	NOTICE							m_eType = {};

	_bool							m_bEnd{ false };
	_float							m_fAlpha{}, m_fTimeTick = {}, m_fTimeRange = {};
	_float4							m_fTexCoord = {};
	shared_ptr<Engine::CShader>		m_pShaderCom = { nullptr };
	shared_ptr<Engine::CRect>		m_pVIBufferCom = { nullptr };
public:
	static		unique_ptr<CWorldUI>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual		shared_ptr<CPrototype>	Clone(void* pArg) override;
};

NS_END

