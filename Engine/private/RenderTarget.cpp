#include "RenderTarget.h"
#include "GameInstance.h"

CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{

}

CRenderTarget::~CRenderTarget()
{
}

HRESULT CRenderTarget::Initialize(uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{

	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), nullptr, &m_pSRV)))
		return E_FAIL;


	m_vClearColor = vClearColor;

	return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV.Get(), reinterpret_cast<_float*>(&m_vClearColor));
}

HRESULT CRenderTarget::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV.Get());
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_float2		vViewportSize = CGameInstance::Get().Get_ViewportSize();

	XMStoreFloat4x4(&m_WorldMatrix,
		XMMatrixScaling(fSizeX, fSizeY, 1.f) *
		XMMatrixTranslation(fX - vViewportSize.x * 0.5f, -fY + vViewportSize.y * 0.5f, 0.f));

	return S_OK;
}

HRESULT CRenderTarget::Debug_Render(shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr<CRect> pVIBuffer)
{ 
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV(pConstantName, m_pSRV.Get())))
		return E_FAIL;

	if (FAILED(pShader->Begin(ETOUI(DEFERRED::DEBUG))))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#endif

shared_ptr<CRenderTarget> CRenderTarget::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	auto	pInstance = shared_ptr<CRenderTarget>(new CRenderTarget(pDevice, pContext));

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
		MSG_BOX("Failed to Created : CRenderTarget");

	return pInstance;
}
