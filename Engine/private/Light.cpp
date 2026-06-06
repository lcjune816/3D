#include "Light.h"
#include "GameInstance.h"
CLight::CLight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext)
{

}

CLight::~CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

    return S_OK;
}

HRESULT CLight::Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer)
{
    uint32_t    iPassIndex = {};

    if (LIGHT::DIRECTIONAL == m_LightDesc.eType)
    {
        if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof m_LightDesc.vDir)))
            return E_FAIL;

        iPassIndex = ETOUI(DEFERRED::DIRECTIOANL);
    }
    else
    {
        iPassIndex = ETOUI(DEFERRED::POINT);
    }

    if (FAILED(pShader->Begin(iPassIndex)))
        return E_FAIL;

    if (FAILED(pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CLight> CLight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const LIGHT_DESC& LightDesc)
{
    auto pInstance = shared_ptr<CLight>(new CLight(pDevice, pContext));
    if (FAILED(pInstance->Initialize(LightDesc)))
    {
        MSG_BOX("Create Failed Light");
        return nullptr;
    }
    return pInstance;
}
