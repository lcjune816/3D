#include "CustomFont.h"

CCustomFont::CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

CCustomFont::~CCustomFont()
{
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = make_unique<SpriteFont>(m_pDevice.Get(), pFontFilePath);
	m_pBatch = make_unique<SpriteBatch>(m_pContext.Get());

	return S_OK;
}

void CCustomFont::Draw(const _tchar* pText, const _float2& vPosition, float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
	m_pBatch->Begin();
	
	m_pFont->DrawString(m_pBatch.get(), pText, vPosition, vColor, fRotation, vOrigin, fScale);

	m_pBatch->End();
}

unique_ptr<CCustomFont> CCustomFont::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pFontFilePath)
{
	auto	pInstance = unique_ptr<CCustomFont>(new CCustomFont(pDevice, pContext));

	if (FAILED(pInstance->Initialize(pFontFilePath)))
		MSG_BOX("Failed to Created : CCustomFont");

	return pInstance;
}
