#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		uint32_t	iNumInstances;
		_float3		fRange;
		_float3		vCenter;
		_float2		vSize;
	}INSTANCE_DESC;
protected:
	CVIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CVIBuffer_Instance();

public:
	virtual HRESULT	 Initialize_Prototype(void* pArg);
	virtual HRESULT  Initialize(void* pArg)override;
	virtual HRESULT	 Bind_Resource()override;
	virtual HRESULT  Render()override;

protected:
	ComPtr<ID3D11Buffer>		m_pVBInstance = { nullptr };
	uint32_t					m_iInstanceVertexStride = {};
	uint32_t					m_iIndexCountPerInstance = {};
	uint32_t					m_iNumInstances = {};

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) PURE;

};

NS_END

