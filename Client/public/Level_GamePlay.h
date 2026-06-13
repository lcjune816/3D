#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CCube;
NS_END


NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_GamePlay();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT		Ready_Lights();
	HRESULT		Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Boss(const _wstring& strLayerTag);

	HRESULT		Ready_Layer_Decal(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_WorldObject(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_TriggerObject(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_WorldObjectInstance(const _wstring& strLayerTag);
	HRESULT		Ready_ProtoType();

	HRESULT		Ready_Layer_Gui(const _wstring& strLayerTag);
private:
	_float			m_fFirstBgm{};

	_bool			m_bEndSound{ false };
public:
	static unique_ptr<CLevel_GamePlay> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END