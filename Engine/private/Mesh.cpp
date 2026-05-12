#include "Mesh.h"
#include "TextureLoader.h"
#include "GameInstance.h"
CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CMesh::~CMesh()
{
}

HRESULT CMesh::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CMesh::Initialize(void* pArg)
{
	
	if (nullptr == pArg)
		return E_FAIL;
	auto Dest = static_cast<MESH_DESC*>(pArg);

	m_iNumVertexBuffers = 1;
	m_iNumVertices		= Dest->m_Mesh._VertexList.size();
	m_iVertexStride		= sizeof(VERTEX);
	
	m_iNumIndices		= Dest->m_Mesh._Indices.size();
	m_iIndexStride		= 4;
	m_eIndexFormat		= DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		
	for(size_t i=0; i< AI_TEXTURE_TYPE_MAX; ++i)
		m_TextureList[i] = move(Dest->m_Mesh._TextureList[i]);
	if (FAILED(Setup_Mesh(Dest->m_Mesh._VertexList,Dest->m_Mesh._Indices)))
		return E_FAIL;

}


HRESULT CMesh::Setup_Mesh(vector<VERTEX>& vertices, vector<uint32_t>& indeces)
{
	D3D11_BUFFER_DESC vbd{};

	vbd.ByteWidth = m_iNumVertices* m_iVertexStride;//정점 개수 * 정점 크기
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = m_iVertexStride;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = &vertices[0];

	if (FAILED(m_pDevice->CreateBuffer(&vbd, &initData, &m_pVB)))
		return E_FAIL;

	D3D11_BUFFER_DESC ibd{};
	ibd.ByteWidth = m_iNumIndices * m_iIndexStride;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.StructureByteStride = m_iIndexStride;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &indeces[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}



unique_ptr<CMesh> CMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CMesh>(new CMesh(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CMesh");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CMesh::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CMesh>(new CMesh(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CMesh_Clone");
		return nullptr;
	}

	return pInstance;
}
