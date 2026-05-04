#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLevel
{
protected:
	CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLevel();

public:
	virtual HRESULT Initialize();
	virtual void	Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>		m_pDevice  = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr};


};

NS_END

