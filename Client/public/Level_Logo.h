#pragma once

#include "Client_Defines.h"
#include "Level.h"


NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual	~CLevel_Logo();

public:
	virtual	HRESULT Initialize() override;
	virtual	void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT		Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_WorldObject(const _wstring& strLayerTag);
	HRESULT		Ready_Layer_Component(const _wstring& strLayerTag);

public:
	static unique_ptr<CLevel_Logo> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
