#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"
CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Particle_Point::~CVIBuffer_Particle_Point()
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(void* pArg)
{
	auto pDesc = static_cast<PARTICLE_INSTANCE_DESC*>(pArg);

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTX_POS);
	m_iNumIndices = 0;
	m_iIndexStride = 0;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iIndexCountPerInstance = m_iNumIndices;
	m_iNumInstances = pDesc->iNumInstances;

	D3D11_BUFFER_DESC		VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	
	unique_ptr<VTX_POS[]> pVertices = make_unique<VTX_POS[]>(m_iNumVertices);

	pVertices[0].fPos = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA		VertexInitialData{};
	VertexInitialData.pSysMem = pVertices.get();

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	m_InstanceBufferDesc.ByteWidth = m_iNumInstances * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceData = shared_ptr<VTXINSTANCE_PARTICLE[]>(new VTXINSTANCE_PARTICLE[m_iNumInstances]);
	m_pSpeeds = shared_ptr<_float[]>(new _float[m_iNumInstances]);
	m_isLoop = pDesc->isLoop;
	m_vPivot = pDesc->vPivot;
	m_vGrid = pDesc->vGrid;
	for (size_t i = 0; i < m_iNumInstances; ++i)
	{
		_float		fScale = CGameInstance::Get().Random(pDesc->vSize.x, pDesc->vSize.y);
		m_pSpeeds[i] = CGameInstance::Get().Random(pDesc->vSpeed.x, pDesc->vSpeed.y);

		m_pInstanceData[i].fRight	    = _float4(fScale , 0.f, 0.f, 0.f);
		m_pInstanceData[i].fUp		    = _float4(0, fScale, 0.f, 0.f);
		m_pInstanceData[i].fLook	    = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceData[i].fTranslation = _float4(
		CGameInstance::Get().Random((pDesc->vCenter.x - pDesc->fRange.x) * 0.5f,(pDesc->vCenter.x + pDesc->fRange.x) * 0.5f),
		CGameInstance::Get().Random((pDesc->vCenter.y - pDesc->fRange.y) * 0.5f,(pDesc->vCenter.y + pDesc->fRange.y) * 0.5f),
		CGameInstance::Get().Random((pDesc->vCenter.z - pDesc->fRange.z) * 0.5f,(pDesc->vCenter.z + pDesc->fRange.z) * 0.5f),
		1.f
		);
		m_pInstanceData[i].fUv = _float4(0.f,0.f, 1.f / pDesc->vGrid.x , 1.f / pDesc->vGrid.y);
		m_pInstanceData[i].fLifeTime = _float2(
			CGameInstance::Get().Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y), 0.f);
		m_pInstanceData[i].fTick = 0.f;
	}

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA		InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceData.get();

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB.Get(),
		m_pVBInstance.Get(),
	};

	uint32_t	iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride
	};

	uint32_t		iOffSets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffSets);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstances, 0, 0);


	return S_OK;
}

void CVIBuffer_Particle_Point::Fog_Spread(const _float& fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

															//기존꺼 유지
	if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
		return;

	auto	pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

	for (uint32_t i = 0; i < m_iNumInstances; ++i)
	{
		pVertices[i].fLifeTime.y += fTimeDelta;

		_vector vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].fTranslation), 0.f));
		XMStoreFloat4(&pVertices[i].fTranslation, XMLoadFloat4(&pVertices[i].fTranslation)+ vDir * m_pSpeeds[i] * fTimeDelta);


		//x y          z          w 
		// 최소      x최대      y최대
	
		if (pVertices[i].fLifeTime.y >= pVertices[i].fLifeTime.x)
		{
			pVertices[i].fLifeTime.y = 0.f;

			pVertices[i].fUv.x = pVertices[i].fUv.z;
			pVertices[i].fUv.z += 1.f / m_vGrid.x;

			if (pVertices[i].fUv.z > 1.f - 0.0001f)
			{
				pVertices[i].fUv.x = 0;
				pVertices[i].fUv.z = 1.f / m_vGrid.x;
				pVertices[i].fUv.y = pVertices[i].fUv.w;
				pVertices[i].fUv.w += 1.f / m_vGrid.y;

			}
			if (pVertices[i].fUv.w > 1.f - 0.0001f)
			{

				pVertices[i].fUv.x = 0.f;
				pVertices[i].fUv.z = 1.f / m_vGrid.x;

				pVertices[i].fUv.y = 0.f;
				pVertices[i].fUv.w = 1.f / m_vGrid.y;

			}
		}
		
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);

}

void CVIBuffer_Particle_Point::Spark(const _float& fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	//기존꺼 유지
	if (FAILED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource)))
		return;

	auto	pVertices = static_cast<VTXINSTANCE_PARTICLE*>(MappedSubResource.pData);

	for (uint32_t i = 0; i < m_iNumInstances; ++i)
	{
		pVertices[i].fLifeTime.y += fTimeDelta;

		_vector vDir = XMVector4Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].fTranslation), 0.f));
		XMStoreFloat4(&pVertices[i].fTranslation, XMLoadFloat4(&pVertices[i].fTranslation) + vDir * m_pSpeeds[i] * fTimeDelta);


		//x y          z          w 
		// 최소      x최대      y최대

		m_pInstanceData[i].fTick += fTimeDelta;
		
		if (m_pInstanceData[i].fTick > 0.1f)
		{
			m_pInstanceData[i].fTick = 0;
			pVertices[i].fUv.x = pVertices[i].fUv.z;
			pVertices[i].fUv.z += 1.f / m_vGrid.x;

			if (pVertices[i].fUv.z > 1.f - 0.0001f)
			{
				pVertices[i].fUv.x = 0;
				pVertices[i].fUv.z = 1.f / m_vGrid.x;
				pVertices[i].fUv.y = pVertices[i].fUv.w;
				pVertices[i].fUv.w += 1.f / m_vGrid.y;

			}
			if (pVertices[i].fUv.w > 1.f - 0.0001f)
			{

				pVertices[i].fUv.x = 0.f;
				pVertices[i].fUv.z = 1.f / m_vGrid.x;

				pVertices[i].fUv.y = 0.f;
				pVertices[i].fUv.w = 1.f / m_vGrid.y;

			}
		
		}
		if (pVertices[i].fLifeTime.y >= pVertices[i].fLifeTime.x)
		{

			pVertices[i].fLifeTime.y = 0.f;
		}

	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Particle_Point::Steam(const _float& fTimeDelta)
{
}


unique_ptr<CVIBuffer_Particle_Point> CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	auto		pInstance = unique_ptr<CVIBuffer_Particle_Point>(new CVIBuffer_Particle_Point(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		return nullptr;
	}

	return pInstance;
}



shared_ptr<CPrototype> CVIBuffer_Particle_Point::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CVIBuffer_Particle_Point>(new CVIBuffer_Particle_Point(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
		return nullptr;
	}

	return pInstance;
}

