#include "Rect.h"

CRect::CRect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CRect::~CRect()
{
}

HRESULT CRect::Initialize_Prototype()
{

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTX_TEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	if (FAILED(Setup_Mesh()))
		return E_FAIL;


	return S_OK;
}
HRESULT CRect::Initialize(void* pArg)
{

	return S_OK;

}


HRESULT CRect::Setup_Mesh()
{

	D3D11_BUFFER_DESC vbd{};

	vbd.ByteWidth = m_iNumVertices * m_iVertexStride;//정점 개수 * 정점 크기
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = m_iVertexStride;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};

	VTX_TEX* pVertices = new VTX_TEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_TEX) * m_iNumVertices);

	initData.pSysMem = &pVertices[0];
	pVertices[0].vPosition   = { -0.5f,0.5f,0.f };
	pVertices[0].vTexcoord = { 0.f,0.f };
	
	pVertices[1].vPosition = {  0.5f,0.5f,0.f };
	pVertices[1].vTexcoord = { 1.f,0.f };

	pVertices[2].vPosition = { 0.5f,-0.5f,0.f };
	pVertices[2].vTexcoord = { 1.f,1.f };

	pVertices[3].vPosition = { -0.5f,-0.5f,0.f };
	pVertices[3].vTexcoord = { 0.f,1.f };


	if (FAILED(m_pDevice->CreateBuffer(&vbd, &initData, &m_pVB)))
		return E_FAIL;

	D3D11_BUFFER_DESC ibd{};
	ibd.ByteWidth = m_iNumIndices * m_iIndexStride;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.StructureByteStride = m_iIndexStride;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	uint16_t* pIndices = new uint16_t[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(uint16_t) * m_iNumIndices);


	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 3;
	pIndices[5] = 2;

	D3D11_SUBRESOURCE_DATA          IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);
	return S_OK;
}



unique_ptr<CRect> CRect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CRect>(new CRect(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CRect");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CRect::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CRect>(new CRect(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CRect_Clone");
		return nullptr;
	}

	return pInstance;
}
