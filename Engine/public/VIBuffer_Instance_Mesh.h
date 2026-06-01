#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instance_Mesh  final : public CVIBuffer_Instance
{
public:
	typedef struct tagMeshInsatnceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		INSTANCING_DESC insatnceData;

	}MESHINSTANCE_DESC;
protected:
	CVIBuffer_Instance_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CVIBuffer_Instance_Mesh();

public:
	virtual HRESULT	 Initialize_Prototype(void* pArg)override;
	virtual HRESULT  Initialize(void* pArg)override;

public:
	static unique_ptr<CVIBuffer_Instance_Mesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};


NS_END
