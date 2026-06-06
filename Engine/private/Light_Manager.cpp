#include "Light_Manager.h"
#include "GameInstance.h"
#include "Light.h"
CLight_Manager::CLight_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{

}
CLight_Manager::~CLight_Manager()
{

}

HRESULT CLight_Manager::Initialize()
{
 
    if (FAILED(Load_LightMtrl()))
        return E_FAIL;

    return S_OK;
}
HRESULT CLight_Manager::Add_LightMtrl(const string tagLight)
{
    auto Light = Find_LightMtrl(tagLight).lock();
    if (NULL_FALSE(Light))
         return S_OK;


    shared_ptr<LIGHT_VALUE> pValue(new LIGHT_VALUE);
    pValue->vLightDir = { 1.f, -1.f, 1.f, 0.f };
    pValue->vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
    pValue->vLightAmbient = { 0.6f, 0.6f, 0.6f, 1.f };

    pValue->vLightSpecular = {1.f, 1.f, 1.f, 1.f};
    pValue->vMtrlAmbient = { 0.4f,0.4f,0.4f,1.f };
    pValue->vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

    m_LightMtrls.try_emplace(tagLight, pValue);
    
    filesystem::path file = tagLight;
    m_ObjectNames.push_back(file.filename().string());

    return S_OK;
}

weak_ptr<LIGHT_VALUE>  CLight_Manager::Find_LightMtrl(const string tagLightName)
{
    auto light = m_LightMtrls.find(tagLightName);

    if (light != m_LightMtrls.end())
    {
        return light->second;
    }
        
    return {};
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    auto pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
    if (NULL_TRUE(pLight))
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}
HRESULT CLight_Manager::Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer)
{
    for (auto& pLight : m_Lights)
    {
        pLight->Render(pShader, pVIBuffer);
    }
    return S_OK;
}
HRESULT CLight_Manager::Load_LightMtrl()
{
    return S_OK;
}


unique_ptr<CLight_Manager> CLight_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = unique_ptr<CLight_Manager>(new CLight_Manager(pDevice, pContext));
    
    if(FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed Light_Manager");
        return nullptr;
    }

    return pInstance;
}