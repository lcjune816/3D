#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CLight final
{
private:
	CLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CLight();

public:
	HRESULT	Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer);

private:
	ComPtr<ID3D11Device>		m_pDevice = { };
	ComPtr<ID3D11DeviceContext>	m_pContext = {};
	
	LIGHT_DESC	m_LightDesc{};

public:
	static		shared_ptr<CLight> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc);
};

NS_END

