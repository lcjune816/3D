#include "Terrain.h"

CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = TERRIANZ * TERRIANX ;
	m_iVertexStride = sizeof(VERTEX_NOANIME);

	m_iNumIndices = (TERRIANZ - 1) * (TERRIANX - 1) * 6 ;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	if (FAILED(Setup_Mesh()))
		return E_FAIL;


	return S_OK;
}
HRESULT CTerrain::Initialize(void* pArg)
{

	return S_OK;

}


HRESULT CTerrain::Setup_Mesh()
{

	D3D11_BUFFER_DESC vbd{};

	vbd.ByteWidth = m_iNumVertices * m_iVertexStride;//정점 개수 * 정점 크기
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = m_iVertexStride;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};

	VERTEX_NOANIME* pVertices = new VERTEX_NOANIME[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VERTEX_NOANIME) * m_iNumVertices);

	for (int32_t i = 0; i < TERRIANZ; ++i)
	{
		for (int32_t j = 0; j < TERRIANX; ++j)
		{
			VERTEX_NOANIME vtx;
			vtx.fPos = { (_float)j * 10, 0.f, _float(i * 10) };
			pVertices[i * TERRIANZ + j] = vtx;
		}
	}

	initData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&vbd, &initData, &m_pVB)))
		return E_FAIL;

	D3D11_BUFFER_DESC ibd{};
	ibd.ByteWidth = m_iNumIndices * m_iIndexStride;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.StructureByteStride = m_iIndexStride;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	int32_t* pIndeces = new int32_t[m_iNumIndices];
	int32_t index = 0;
	for (int32_t i = 0; i < TERRIANZ - 1; ++i)
	{
		
		for (int32_t j = 0; j < TERRIANX - 1; ++j)
		{
			pIndeces[index++] = j + TERRIANZ * i;
			
			pIndeces[index++] = (j + 1) + TERRIANZ * i;
			pIndeces[index++] = (i + 1) * TERRIANZ + j;

			pIndeces[index++] = (j + 1) + TERRIANZ * i;
			pIndeces[index++] = (i + 1) * TERRIANZ + j;
			pIndeces[index++] = (i + 1) * TERRIANZ + (j + 1);

		}
	}


	initData.pSysMem =	pIndeces;

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndeces);
	Safe_Delete_Array(pVertices);
	return S_OK;
}



unique_ptr<CTerrain> CTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CTerrain>(new CTerrain(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed 3DModel");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CTerrain::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CTerrain>(new CTerrain(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("3DMdoel Create Failed");
		return nullptr;
	}

	return pInstance;
}
