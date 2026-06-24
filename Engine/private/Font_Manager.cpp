#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

CFont_Manager::~CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    auto    pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Font.emplace(strFontTag, move(pFont));

    return S_OK;
}

void CFont_Manager::Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    auto        pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return;

    m_pContext->GSSetShader(nullptr, nullptr, 0);

    pFont->Draw(pText, vPosition, fScale, vColor, fRotation, vOrigin);
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Font.find(strFontTag);
    if (iter == m_Font.end())
        return nullptr;

    return iter->second.get();
}

unique_ptr<CFont_Manager> CFont_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return unique_ptr<CFont_Manager>(new CFont_Manager(pDevice, pContext));
}