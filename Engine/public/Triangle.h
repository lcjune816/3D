#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTriAngle final : public CVIBuffer
{
private:
	CTriAngle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CTriAngle();


public:
	virtual		HRESULT			Initialize_Prototype();
	virtual		HRESULT			Initialize(void* pArg) override;
	HRESULT			Setup_Mesh(vector<_float3[3]>& Vertices, vector<uint32_t[3]>& indices);

private:
	ComPtr<ID3DX11Effect>		m_pEffect = { nullptr };

	uint32_t					m_iNumPasses = {};
	vector<ComPtr<ID3D11InputLayout>> m_InputLayouts;

public:
	static unique_ptr<CTriAngle> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	shared_ptr<CPrototype> Clone(void* pArg);

};


NS_END
