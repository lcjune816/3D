#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext>pContext) 
	:CComponent(pDevice, pContext)
{

}

CShader::~CShader()
{

}

HRESULT CShader::Initialize_Prototype(const _wstring pShaderFilePath,
	const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements)
{
	uint32_t	iFlag = {};

#ifdef _DEBUG
	iFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
								//쉐이더 파일경로					//쉐이더 파일 내에서 인쿠르드 가능하게					
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
		return E_FAIL;

	//이펙트에서 테크닉을 하나 가져온다
	//테크닉은 어떤 방식으로 그릴지에 대한 전체 세트
	ComPtr<ID3DX11EffectTechnique>		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};
	pTechnique->GetDesc(&TechniqueDesc);

	//정점 데이터 -> inputlayout -> 버텍스 셰이더 -> 픽셀 셰이더 -> 화면 출력
	//그 테크닉 내부 전체세트 기준으로
	// 몇개의 패스가 있는지 확인 해당 오브젝트를 몇번 그릴건지에대한 카운트
	// 랜더링을 몇단계로 나눌건지에 대한 거라고 보면 될것같음
	m_iNumPasses = TechniqueDesc.Passes;
	
	for (uint32_t i = 0; i < m_iNumPasses; ++i)
	{
		ComPtr<ID3D11InputLayout>		pInputLayout = { nullptr };

		D3DX11_PASS_DESC		PassDesc{};

		ComPtr<ID3DX11EffectPass>	pPass = pTechnique->GetPassByIndex(i);

		pPass->GetDesc(&PassDesc);
		//각 패스 버텍스 셰이더 입력 시그니처를 얻는다
		//셰이더가 요구하는 입력 정보를 긁어오는것
		PassDesc.pIAInputSignature;
		PassDesc.IAInputSignatureSize;

		//레이아웃 생성
		if (FAILED(m_pDevice->CreateInputLayout(pInputElements, iNumElements,
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);

	}

	return S_OK;

}
HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(uint32_t iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;
	_float4		Reset{ 0,0,0,0 };
	//m_pContext->OMSetBlendState(nullptr, (_float*)&Reset, 0xffffffff);
	//m_pContext->RSSetState(nullptr);
	//m_pContext->IASetInputLayout(nullptr);
	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex].Get());

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext.Get());

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	if (NULL_TRUE(m_pEffect))
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (NULL_TRUE(pVariable))
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (NULL_TRUE(pMatrixVariable))
		return E_FAIL;
	
	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrix_Array(const _char* pConstantName, const _float4x4* Mat, const uint32_t iBoneCnt)
{
	if (NULL_TRUE(m_pEffect))
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (NULL_TRUE(pVariable))
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (NULL_TRUE(pMatrixVariable))
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(Mat),0,iBoneCnt);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* fColor, uint32_t iSize)
{
	if (NULL_TRUE(m_pEffect))
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (NULL_TRUE(pVariable))
		return E_FAIL;

	return pVariable->SetRawValue(fColor,0,iSize);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{

	if (NULL_TRUE(m_pEffect))
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (NULL_TRUE(pVariable))
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (NULL_TRUE(pShaderResourceVariable))
		return E_FAIL;

	return pShaderResourceVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_Vector_Array(const _char* pConstantName, const _float4* Mat, const uint32_t iBoneCnt)
{
	if (NULL_TRUE(m_pEffect))
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (NULL_TRUE(pVariable))
		return E_FAIL;

	ID3DX11EffectVectorVariable* pMatrixVariable = pVariable->AsVector();
	if (NULL_TRUE(pMatrixVariable))
		return E_FAIL;

	return pMatrixVariable->SetFloatVectorArray(reinterpret_cast<const _float*>(Mat), 0, iBoneCnt);
}

unique_ptr<CShader>		CShader::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, const _wstring pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements)
{
	auto		pInstance = unique_ptr<CShader>(new CShader(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pInputElements, iNumElements)))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CShader::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CShader>(new CShader(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}