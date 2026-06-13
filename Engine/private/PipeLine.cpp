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

HRESULT CPipeLine::MultiByteWstringToChar(const _wstring& strConvertwstring,  string& strDestName)
{
    size_t iSize = WideCharToMultiByte(CP_UTF8,0,strConvertwstring.c_str(),-1, NULL,0, NULL, NULL);
    string ConvertString(iSize,0);
    
    WideCharToMultiByte(CP_UTF8, 0, strConvertwstring.data(), -1, ConvertString.data(), iSize  , NULL, NULL);

    strDestName = (ConvertString.data());
    if (strDestName.empty())
        return E_FAIL;

    return S_OK;
}

HRESULT CPipeLine::MultiByteCharToWstring(const string& strConvertstring ,  _wstring& strDest)
{

    size_t iSize = MultiByteToWideChar(CP_UTF8, 0, strConvertstring.c_str(),-1 , NULL, 0);
    _wstring ConvertWstring(iSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, strConvertstring.c_str(), -1, ConvertWstring.data(), iSize);

    strDest = ConvertWstring.data();
    
    if (strDest.empty())
        return E_FAIL;

    return S_OK;
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