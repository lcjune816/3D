#include "NaviMesh.h"
#include "GameInstance.h"
CNaviMesh::CNaviMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CNaviMesh::~CNaviMesh()
{
}

HRESULT CNaviMesh::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = CGameInstance::Get().Get_MeshInfo().size();
	m_iVertexStride = sizeof(VERTEX_BOX);

	m_iNumIndices = CGameInstance::Get().Get_MeshInfo().size() * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	if (FAILED(Setup_Mesh()))
		return E_FAIL;


	return S_OK;
}
HRESULT CNaviMesh::Initialize(void* pArg)
{

	return S_OK;

}


HRESULT CNaviMesh::Setup_Mesh()
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

	VERTEX_BOX* pVertices = new VERTEX_BOX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VERTEX_BOX) * m_iNumVertices);

	int32_t* pIndeces = new int32_t[m_iNumIndices];
	for (size_t i = 0; i < CGameInstance::Get().Get_MeshInfo().size(); ++i)
	{
		pVertices[i].fPos = { CGameInstance::Get().Get_MeshInfo()[i].fPos.x,
							CGameInstance::Get().Get_MeshInfo()[i].fPos.y +0.5f,
							CGameInstance::Get().Get_MeshInfo()[i].fPos.z };

		pVertices[i].fColor = { 0.f,1.f,0.f,1.f };

		pIndeces[i]   = (i * 3);
		pIndeces[i+1] = (i * 3 + 1);
		pIndeces[i+2] = (i * 3 + 2);
	}
	initData.pSysMem = &pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&vbd, &initData, &m_pVB)))
		return E_FAIL;

	D3D11_BUFFER_DESC ibd{};
	ibd.ByteWidth = m_iNumIndices * m_iIndexStride;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.StructureByteStride = m_iIndexStride;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &pIndeces[0];

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &initData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndeces);
	Safe_Delete_Array(pVertices);
	return S_OK;
}



unique_ptr<CNaviMesh> CNaviMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CNaviMesh>(new CNaviMesh(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CNaviMesh");
		return nullptr;
	}
	return pInstance;
}

shared_ptr<CPrototype> CNaviMesh::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CNaviMesh>(new CNaviMesh(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CNaviMesh_Clone");
		return nullptr;
	}

	return pInstance;
}
