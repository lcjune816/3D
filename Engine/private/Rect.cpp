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
	m_iVertexStride = sizeof(VERTEX_NOANIME);

	m_iNumIndices = 6;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
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
	pVertices[0].fPos   = { -1.f,1.f,0.f };
	pVertices[0].texcoord = { 0,1 };
	
	pVertices[1].fPos = {  1.f,1.f,0.f };
	pVertices[1].texcoord = { 1,1 };

	pVertices[2].fPos = { 1.f,-1.f,0.f };
	pVertices[2].texcoord = { 1,0 };

	pVertices[3].fPos = { -1.f,-1.f,0.f };
	pVertices[3].texcoord = { 0,0 };


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
	pIndeces[0] = 0;  
	pIndeces[1] = 3; 
	pIndeces[2] = 2;

	pIndeces[3] = 0;
	pIndeces[4] = 1;
	pIndeces[5] = 2;
	initData.pSysMem = &pIndeces[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndeces);
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
