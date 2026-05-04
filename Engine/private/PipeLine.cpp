#include "PipeLine.h"

CPipeLine::~CPipeLine()
{
}
CPipeLine::CPipeLine()
{

}

const _float4x4* CPipeLine::Get_Transform(D3DTS eState)
{
    return &m_TransformStateMatrices[ETOUI(eState)];
}

const _float4x4* CPipeLine::Get_Transform_Inverse(D3DTS eState)
{
    return &m_TransformStateInverseMatrices[ETOUI(eState)];
}

const _float4* CPipeLine::Get_CamPosition()
{
    return &m_vCamPosition;
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(eState)], TransformMatrix);
}

HRESULT CPipeLine::Initialize()
{
    for (uint32_t i = 0; i < ETOUI(D3DTS::END); ++i)
    {
        XMStoreFloat4x4(&m_TransformStateMatrices[i],XMMatrixIdentity());
        XMStoreFloat4x4(&m_TransformStateInverseMatrices[i], XMMatrixIdentity());
    }

    return S_OK;
}

void CPipeLine::Update()
{
    for (uint32_t i = 0; i < ETOUI(D3DTS::END); ++i)
    {
        XMStoreFloat4x4(&m_TransformStateInverseMatrices[i],
            XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformStateMatrices[i])));
    }
    memcpy(&m_vCamPosition, &m_TransformStateInverseMatrices[ETOUI(D3DTS::VIEW)]._41,sizeof(_float4));
}

unique_ptr<CPipeLine> CPipeLine::Create()
{
    auto    pInstance = unique_ptr<CPipeLine>(new CPipeLine());

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CPipeLine");

    return pInstance;
}