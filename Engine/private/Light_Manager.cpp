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
LIGHT_DESC* CLight_Manager::Find_LightMtrl(const LIGHT& eLight)
{
    LIGHT_DESC* pDesc = nullptr;
    for (auto& Light : m_Lights)
    {
        if (pDesc = Light->Get_LightDesc(eLight))
        {
            return pDesc;
        }
    }
    return nullptr;
}

void CLight_Manager::Set_LightDesc(LIGHT_DESC& eLight)
{
    for(auto& Light : m_Lights)
    {
        Light->Set_LightDesc(eLight);
    }
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