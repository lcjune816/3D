#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CTarget_Manager final
{
private:
	CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CTarget_Manager();

//멀티 랜더 타겟
public:
	HRESULT Add_RenderTarger(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelForamt, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_ShaderResource(const _wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float SizeX, _float SizeY);
	HRESULT Debug_Render(const _wstring& strMRTTag, shared_ptr<class CShader> pShader, const _char* pConstantName, shared_ptr<class CRect> pVIBuffer);

#endif

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };
	ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV = { nullptr };
	ComPtr<ID3D11DepthStencilView> m_pOriginalDSV = { nullptr };

private:
	map<const _wstring, shared_ptr<class CRenderTarget>> m_RenderTargets;
	map<const _wstring, list<shared_ptr<class CRenderTarget>>> m_MRTs;

private:
	shared_ptr<class CRenderTarget> Find_RenderTarget(const _wstring& strTargetTag);
	list<shared_ptr<class CRenderTarget>>* Find_MRT(const _wstring& strMRTTag);

public:
	static unique_ptr<CTarget_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END