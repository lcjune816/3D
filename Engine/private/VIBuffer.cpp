#include "VIBuffer.h"
#include "GameInstance.h"
CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice,pContext)
{

}
CVIBuffer::~CVIBuffer()
{

}


HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}


HRESULT CVIBuffer::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB.Get(),
	};

	uint32_t	iVertexStrides[] = {
		m_iVertexStride,
	};

	uint32_t		iOffsets[] = {
		0,
	};

	//if (!m_TextureList.empty())
	//{
	//	auto iter = CGameInstance::Get().Find_Texture(m_TextureList[0].path);
	//	m_pContext->PSSetShaderResources(0, 1, &iter);
	//}
	
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Resource_InstanceCount(const uint32_t iNumber)
{
	m_pContext->DrawIndexedInstanced(1, iNumber, 0, 0, 0);
	return S_OK;
}

HRESULT CVIBuffer::Bind_ResourceSRV(CShader* pShader, const _char* pName, aiTextureType eType, uint32_t iTextureNumber)
{
	if (!m_TextureList[eType].empty())
	{
		pShader->Bind_SRV(pName, CGameInstance::Get().Find_Texture(m_TextureList[eType][iTextureNumber].iTextureID));
	}
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;
	
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
}

