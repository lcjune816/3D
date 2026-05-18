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
	m_iNumVertices = 3;
	m_iVertexStride = sizeof(VTX_POS);

	m_iNumIndices = 4;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	if (FAILED(Setup_Mesh(&pDesc->fPos[0])))
		return E_FAIL;

	return S_OK;

}


HRESULT CTriAngle::Setup_Mesh(_float3* fPos)
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

	VTX_POS* pVertices = new VTX_POS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTX_POS) * m_iNumVertices);

	memcpy(pVertices, fPos, sizeof(VTX_POS) * m_iNumVertices);
	initData.pSysMem = &pVertices[0];

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

	D3D11_SUBRESOURCE_DATA          IndexInitialData{};
	IndexInitialData.pSysMem = &pIndices[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &IndexInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
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
