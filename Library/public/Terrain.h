#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTerrain : public  CVIBuffer
{
public:
	typedef struct tagMeshdesc
	{
		MESH m_Mesh;

	}MESH_DESC;

private:
	CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CTerrain();

	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

private:
	HRESULT							Setup_Mesh();

public:
	static  unique_ptr<CTerrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END