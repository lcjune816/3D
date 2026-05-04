#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CRect final : public CVIBuffer
{
private:
	CRect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CRect();


public:
	virtual		HRESULT			Initialize_Prototype();
	virtual		HRESULT			Initialize(void* pArg) override;
	HRESULT			Setup_Mesh();

private:
	ComPtr<ID3DX11Effect>		m_pEffect = { nullptr };

	uint32_t					m_iNumPasses = {};
	vector<ComPtr<ID3D11InputLayout>> m_InputLayouts;

public:
	static unique_ptr<CRect> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	shared_ptr<CPrototype> Clone(void* pArg);

};


NS_END
