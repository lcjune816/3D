#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CCube;
NS_END


NS_BEGIN(Client)

class CLevel_GasProduction final : public CLevel
{
private:
	CLevel_GasProduction(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel_GasProduction();

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT		Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Decal(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_WorldObject(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_TriggerObject(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Boss(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Gui(const _wstring& strLayerTag);
	HRESULT		Ready_Partilce();
	HRESULT		Ready_ProtoType();
public:
	static unique_ptr<CLevel_GasProduction> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END