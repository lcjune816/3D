#pragma once

#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CShader();


public:
	virtual		HRESULT			Initialize_Prototype(const _wstring pShaderFilePath,
		const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements);
	virtual		HRESULT			Initialize(void* pArg) override;
	

public:
	HRESULT	Begin(uint32_t iPassIndex);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* Mat);
	HRESULT Bind_Matrix_Array(const _char* pConstantName, const _float4x4* Mat, const uint32_t iBoneCnt);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* fColor, uint32_t iSize);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_Vector_Array(const _char* pConstantName, const _float4* Mat, const uint32_t iBoneCnt);
private:
	ComPtr<ID3DX11Effect>		m_pEffect = { nullptr };

	uint32_t					m_iNumPasses = {};
	vector<ComPtr<ID3D11InputLayout>> m_InputLayouts;

public:
	static unique_ptr<CShader> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, const _wstring pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements);
	virtual	shared_ptr<CPrototype> Clone(void* pArg);

};


NS_END
