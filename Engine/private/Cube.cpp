#include "Cube.h"

CCube::CCube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CCube::~CCube()
{
}

HRESULT CCube::Initialize_Prototype()
{

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VERTEX_BOX);

	m_iNumIndices = 36;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	if (FAILED(Setup_Mesh()))
		return E_FAIL;

		
	return S_OK;
}
HRESULT CCube::Initialize(void* pArg)
{

	return S_OK;

}


HRESULT CCube::Setup_Mesh()
{
	_float fScale = 0.5f;
	
	D3D11_BUFFER_DESC vbd{};

	vbd.ByteWidth = m_iNumVertices * m_iVertexStride;//Á¤Á¡ °³¼ö * Á¤Á¡ Å©±â
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = m_iVertexStride;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};

	VERTEX_BOX* pVertices = new VERTEX_BOX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VERTEX_BOX)* m_iNumVertices);
	pVertices[0].fPos = { -fScale, -fScale * 2.f, -fScale };    // ÁÂÇÏÇÏ..   0
	pVertices[0].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[1].fPos = { fScale, -fScale * 2.f, -fScale };    // ¿ìÇÏÇÏ..    1
	pVertices[1].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[2].fPos = { -fScale, fScale * 2.f, -fScale };    // ÁÂ»ó»ó..    2
	pVertices[2].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[3].fPos = { fScale, fScale * 2.f, -fScale };    // ¿ì»ó»ó..     3
	pVertices[3].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[4].fPos = { -fScale, -fScale * 2.f, fScale };    // ÁÂÇÏÇÏ..   4
	pVertices[4].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[5].fPos = { fScale, -fScale * 2.f, fScale };    // ÁÂÇÏÇÏ..  5
	pVertices[5].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[6].fPos = { -fScale, fScale * 2.f, fScale };    // ÁÂÇÏÇÏ..   6
	pVertices[6].fColor = { 1.f, 1.f, 1.f, 1.f };

	pVertices[7].fPos = { fScale, fScale * 2.f, fScale };    // ÁÂÇÏÇÏ..   7
	pVertices[7].fColor = { 0.f, 0.f, 0.f, 1.f };

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

	int32_t* pIndeces = new int32_t[m_iNumIndices];
	pIndeces[0] = 2;  pIndeces[3] = 2;
	pIndeces[1] = 0;  pIndeces[4] = 3;
	pIndeces[2] = 1;  pIndeces[5] = 1;

	pIndeces[6] = 2;  pIndeces[9]  = 2;
	pIndeces[7] = 0;  pIndeces[10] = 6;
	pIndeces[8] = 4;  pIndeces[11] = 4;

	pIndeces[12] = 6; pIndeces[15] = 6;
	pIndeces[13] = 4; pIndeces[16] = 7;
	pIndeces[14] = 5; pIndeces[17] = 5;
	
	pIndeces[18] = 4; pIndeces[21] = 4;
	pIndeces[19] = 0; pIndeces[22] = 5;
	pIndeces[20] = 1; pIndeces[23] = 1;
	
	pIndeces[24] = 6; pIndeces[27] = 6;
	pIndeces[25] = 2; pIndeces[28] = 7;
	pIndeces[26] = 3; pIndeces[29] = 3;
	
	pIndeces[30] = 3; pIndeces[33] = 3;
	pIndeces[31] = 1; pIndeces[34] = 7;
	pIndeces[32] = 5; pIndeces[35] = 5;

	initData.pSysMem = &pIndeces[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndeces);
	Safe_Delete_Array(pVertices);
	return S_OK;
}



unique_ptr<CCube> CCube::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CCube>(new CCube(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CCube");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CCube::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CCube>(new CCube(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CCube_Clone");
		return nullptr;
	}

	return pInstance;
}
