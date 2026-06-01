#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Instance::~CVIBuffer_Instance()
{
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Resource()
{
    ID3D11Buffer* pVertexBuffers[] =
    {
        m_pVB.Get(), //일반 버텍스 데이터 버퍼 ( 위치, 노말, UV 등
        m_pVBInstance.Get(), // 인스턴스용 버퍼 (행렬 offset 등
    };

    uint32_t  iVertexStrides[] = {
        m_iVertexStride,      //정점당 몇바이트 건너뛸지
        m_iInstanceVertexStride
    };
   
    uint32_t    iOffSets[] = {
        0,
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffSets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);



    return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);
    return S_OK;
}
