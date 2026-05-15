#include "Triangle.h"

CTriAngle::CTriAngle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CTriAngle::~CTriAngle()
{
}

HRESULT CTriAngle::Initialize_Prototype()
{


	return S_OK;
}
HRESULT CTriAngle::Initialize(void* pArg)
{
	auto pDesc = static_cast<BUFFER_DESC*>(pArg);

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pDesc->iTriVertex.size() * 3;
	m_iVertexStride = sizeof(VERTEX_NOANIME);

	m_iNumIndices = pDesc->iTriindex.size() * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	if (FAILED(Setup_Mesh(pDesc->iTriVertex, pDesc->iTriindex)))
		return E_FAIL;

	return S_OK;

}


HRESULT CTriAngle::Setup_Mesh(vector<_float3[3]>& Vertices, vector<uint32_t[3]>& indices)
{
	_float fScale = 0.5f;

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

	initData.pSysMem = &pVertices[0];
	for (size_t i = 0; i < Vertices.size(); ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			pVertices[i].fPos = Vertices[i][j];
		}
	}

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
	for (size_t i = 0; i < indices.size(); ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			pIndeces[i]= indices[i][j];
		}
	}

	initData.pSysMem = &pIndeces[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndeces);
	Safe_Delete_Array(pVertices);
	return S_OK;
}



unique_ptr<CTriAngle> CTriAngle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CTriAngle>(new CTriAngle(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CTriAngle");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CTriAngle::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CTriAngle>(new CTriAngle(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CTriAngle_Clone");
		return nullptr;
	}

	return pInstance;
}
