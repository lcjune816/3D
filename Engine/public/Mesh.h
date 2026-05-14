#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine) 

class  CMesh : public  CVIBuffer
{
public:
typedef struct tagMeshdesc
{
	MESH m_Mesh;

}MESH_DESC;

private:
	CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	
public:
	virtual ~CMesh();

	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

private:
	HRESULT							Setup_Mesh(vector<VERTEX>& vertices, vector<uint32_t>& indeces );
private:


public:
	static  unique_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END