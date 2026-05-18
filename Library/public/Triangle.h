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
	HRESULT						Setup_Mesh(_float3* fPos);

public:
	static unique_ptr<CTriAngle> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual	shared_ptr<CPrototype> Clone(void* pArg);

};


NS_END
