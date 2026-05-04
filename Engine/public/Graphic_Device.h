#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CGraphic_Device
{
private:
	CGraphic_Device();
public:
	~CGraphic_Device();

public:
	HRESULT Initialize(HWND hWnd, WINMODE eWinMode, uint32_t iWinSizeX, uint32_t iWinSizeY, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);
	
	HRESULT	Clear_BackBuffer_View(const _float4* pClearColor);

	HRESULT	Clear_DepthStencil_View();

	HRESULT Present();

	void Shutdown();

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };
	ComPtr<IDXGISwapChain>				m_pSwapChain = { nullptr };

	ComPtr<ID3D11RenderTargetView>		m_pBackBufferRTV = { nullptr };
	ComPtr<ID3D11DepthStencilView>		m_pDepthStencilView = { nullptr };

private:

	HRESULT	Ready_SwapChain(HWND hwnd, WINMODE isWindowed, uint32_t iWinCX, uint32_t iWinCY);
	HRESULT	Ready_BackBufferRenderTargetView();
	HRESULT	Ready_DepthStencilView(uint32_t iWinCX, uint32_t iWinCY);

public:
	static unique_ptr<CGraphic_Device> Create(HWND hwnd, WINMODE eWinMode, uint32_t iWinSizeX, uint32_t iWinSizeY, ComPtr<ID3D11Device>& pOutDevice, ComPtr<ID3D11DeviceContext>& pOutContext);

};

NS_END
