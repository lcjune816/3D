#include "VIBuffer_Instance_Mesh.h"

CVIBuffer_Instance_Mesh::CVIBuffer_Instance_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CVIBuffer_Instance(pDevice,pContext)
{
}

CVIBuffer_Instance_Mesh::~CVIBuffer_Instance_Mesh()
{
}

HRESULT CVIBuffer_Instance_Mesh::Initialize_Prototype(void* pArg)
{
	
	return S_OK;
}

HRESULT CVIBuffer_Instance_Mesh::Initialize(void* pArg)
{
	auto pDesc = static_cast<MESHINSTANCE_DESC*>(pArg);

	m_iNumVertexBuffers = 2; //사용할 버퍼 개수
	m_iNumVertices   = pDesc->insatnceData.Vertices; //정점 개수
	m_iVertexStride  = sizeof(VERTEX_NOANIME);
	m_iNumIndices    = pDesc->insatnceData.indices;
	m_iIndexStride   = 4;//인덱스 크기
	m_eIndexFormat   = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_MESH);
	m_iIndexCountPerInstance = m_iNumIndices;
	m_iNumInstances = pDesc->insatnceData.matWorlds.size();

	m_pVB = pDesc->insatnceData.pVB;
	m_pIB = pDesc->insatnceData.pIB;
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		m_TextureList[i] = move(pDesc->insatnceData.Textures[i]);


	D3D11_BUFFER_DESC		InstanceBufferDesc{};
	InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceVertexStride;
	InstanceBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	InstanceBufferDesc.CPUAccessFlags = 0;
	InstanceBufferDesc.MiscFlags = 0;

	auto pInstanceVtx = make_unique<VTXINSTANCE_MESH[]>(m_iNumInstances);

	for (uint32_t i = 0; i < m_iNumInstances; ++i)
	{

		memcpy(&pInstanceVtx[i].fRight,       &pDesc->insatnceData.matWorlds[i].m[0], sizeof _float4);
		memcpy(&pInstanceVtx[i].fUp,          &pDesc->insatnceData.matWorlds[i].m[1], sizeof _float4);
		memcpy(&pInstanceVtx[i].fLook,        &pDesc->insatnceData.matWorlds[i].m[2], sizeof _float4);
		memcpy(&pInstanceVtx[i].fTranslation, &pDesc->insatnceData.matWorlds[i].m[3], sizeof _float4);
	}
	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = pInstanceVtx.get();
	
	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}


unique_ptr<CVIBuffer_Instance_Mesh> CVIBuffer_Instance_Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	auto		pInstance = unique_ptr<CVIBuffer_Instance_Mesh>(new CVIBuffer_Instance_Mesh(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Instance_Mesh");
		return nullptr;
	}

	return pInstance;
}



shared_ptr<CPrototype> CVIBuffer_Instance_Mesh::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CVIBuffer_Instance_Mesh>(new CVIBuffer_Instance_Mesh(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Instance_Mesh");
		return nullptr;
	}

	return pInstance;
}

