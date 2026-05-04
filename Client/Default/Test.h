#pragma once
#pragma once
#include <d3d11.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

struct DX11_TEST_CONTEXT
{
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* pRTV = nullptr;
};

inline void Safe_Release(IUnknown* p)
{
	if (p)
		p->Release();
}

inline HRESULT Init_DX11_Test(HWND hWnd, UINT iWidth, UINT iHeight, DX11_TEST_CONTEXT& tOutContext)
{
	if (nullptr == hWnd)
		return E_FAIL;

	ZeroMemory(&tOutContext, sizeof(DX11_TEST_CONTEXT));

	DXGI_SWAP_CHAIN_DESC tSwapChainDesc = {};
	tSwapChainDesc.BufferDesc.Width = iWidth;
	tSwapChainDesc.BufferDesc.Height = iHeight;
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	tSwapChainDesc.SampleDesc.Count = 1;
	tSwapChainDesc.SampleDesc.Quality = 0;

	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.OutputWindow = hWnd;
	tSwapChainDesc.Windowed = TRUE;
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	tSwapChainDesc.Flags = 0;

	UINT iCreateFlags = 0;

#ifdef _DEBUG
	iCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL eCreatedFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		iCreateFlags,
		&eFeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&tSwapChainDesc,
		&tOutContext.pSwapChain,
		&tOutContext.pDevice,
		&eCreatedFeatureLevel,
		&tOutContext.pContext
	);

	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = tOutContext.pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	);
	if (FAILED(hr))
	{
		Safe_Release(tOutContext.pSwapChain);
		Safe_Release(tOutContext.pContext);
		Safe_Release(tOutContext.pDevice);
		return hr;
	}

	hr = tOutContext.pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&tOutContext.pRTV
	);
	Safe_Release(pBackBuffer);

	if (FAILED(hr))
	{
		Safe_Release(tOutContext.pSwapChain);
		Safe_Release(tOutContext.pContext);
		Safe_Release(tOutContext.pDevice);
		return hr;
	}

	tOutContext.pContext->OMSetRenderTargets(1, &tOutContext.pRTV, nullptr);

	D3D11_VIEWPORT tViewport = {};
	tViewport.TopLeftX = 0.f;
	tViewport.TopLeftY = 0.f;
	tViewport.Width = static_cast<float>(iWidth);
	tViewport.Height = static_cast<float>(iHeight);
	tViewport.MinDepth = 0.f;
	tViewport.MaxDepth = 1.f;

	tOutContext.pContext->RSSetViewports(1, &tViewport);

	return S_OK;
}

inline void Render_DX11_Test(DX11_TEST_CONTEXT& tContext)
{
	if (nullptr == tContext.pContext || nullptr == tContext.pRTV)
		return;

	const float vClearColor[4] = { 0.1f, 0.2f, 0.4f, 1.f };

	tContext.pContext->OMSetRenderTargets(1, &tContext.pRTV, nullptr);
	tContext.pContext->ClearRenderTargetView(tContext.pRTV, vClearColor);

	if (tContext.pSwapChain)
		tContext.pSwapChain->Present(1, 0);
}

inline void Release_DX11_Test(DX11_TEST_CONTEXT& tContext)
{
	Safe_Release(tContext.pRTV);
	Safe_Release(tContext.pSwapChain);
	Safe_Release(tContext.pContext);
	Safe_Release(tContext.pDevice);

	tContext.pRTV = nullptr;
	tContext.pSwapChain = nullptr;
	tContext.pContext = nullptr;
	tContext.pDevice = nullptr;
}