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

#ifdef _DEBUG
        m_pDebugShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DebugBox.hlsl"), VERTEX_BOX::Elements, VERTEX_BOX::iNumElements);
        m_pDebugVIBuffer = CCube::Create(m_pDevice, m_pContext);
#endif
    return S_OK;
}
LIGHT_DESC* CLight_Manager::Find_LightMtrl(const LIGHT& eLight)
{
    LIGHT_DESC* pDesc = nullptr;
  
    for (auto& Light : m_Lights[ETOUI(USETYPE::ENGINE)])
    {
        if (NULL_FALSE(Light))
        {
            if (pDesc = Light->Get_LightDesc(eLight))
            {
                return pDesc;
            }
        }
       
    }
    
    return nullptr;
}

void CLight_Manager::Set_LightDesc(LIGHT_DESC& eLight)
{
    
    for (auto& Light : m_Lights[ETOUI(eLight.eUseType)])
    {
        if (NULL_FALSE(Light))
        {
            if (Light->Get_LightDesc(eLight.eType))
                Light->Set_LightDesc(eLight);

        }
    }
    
    
}


LIGHT_HANDLE CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{

    LIGHT_HANDLE LightHandle{UINT_MAX, UINT_MAX};
    uint32_t iType = ETOUI(LightDesc.eUseType);
    auto pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);
    if (NULL_TRUE(pLight))
        return LightHandle;
    
    if (iType == ETOUI(USETYPE::ENGINE))
    {
        m_Lights[iType].push_back(pLight);
        return { UINT_MAX, UINT_MAX };
    }
        
    uint32_t iIndex = m_Lights[iType].size();
    if (!m_FreeList.empty())
    {
        iIndex = m_FreeList.back();
        m_Lights[iType][iIndex] = pLight;
        LightHandle.iHandle = ++m_Generation[iIndex];
        LightHandle.iIndex = iIndex;
        m_FreeList.pop_back();

    }
    else
    {
        LightHandle.iHandle = 0;
        LightHandle.iIndex = iIndex;
        m_Generation.push_back(0);
        m_Lights[iType].push_back(pLight);
    }
    
    
    return LightHandle;
}
HRESULT CLight_Manager::Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer)
{
    for (uint32_t i = 0; i < ETOUI(USETYPE::END); ++i)
    {
        for (auto& pLight : m_Lights[i])
        {
            if(NULL_FALSE(pLight))
            pLight->Render(pShader, pVIBuffer);
        }
    }
    

    return S_OK;
}
void CLight_Manager::Save_Lights(const _wstring& path, const string& strJsonKeyName)
{
    json j;

    for (uint32_t i = 0; i < ETOUI(USETYPE::END); ++i)
    {
        for (auto& iter : m_Lights[i])
        {
            if(NULL_FALSE(iter))
                j[strJsonKeyName].push_back(iter->Save_Data());
        }
    }

    ofstream file(path);
    file << j.dump(4);

    file.close();

    MSG_BOX("라이트 된듯?");
}
HRESULT CLight_Manager::Load_Lights(uint32_t iPrototypeLevel, const wstring& strPrototypeName, uint32_t iLevel, const _wstring& strLevelName, json& j)
{
 
    for (auto& iter : j["Lights"])
    {
        LIGHT_HANDLE LightHandle{ UINT_MAX, UINT_MAX };
        uint32_t iType = iter["UseType"];
        auto pLight = CLight::Create(m_pDevice, m_pContext, iter);
        if (NULL_TRUE(pLight))
            return E_FAIL;

        if (iType == ETOUI(USETYPE::ENGINE))
        {
            m_Lights[iType].push_back(pLight);
            continue;
        }

        uint32_t iIndex = m_Lights[iType].size();
        if (!m_FreeList.empty())
        {
            iIndex = m_FreeList.back();
            m_Lights[iType][iIndex] = pLight;
            LightHandle.iHandle = ++m_Generation[iIndex];
            LightHandle.iIndex = iIndex;
            m_FreeList.pop_back();

        }
        else
        {
            LightHandle.iHandle = 0;
            LightHandle.iIndex = iIndex;
            m_Generation.push_back(0);
            m_Lights[iType].push_back(pLight);
        }

        if (iType == ETOUI(USETYPE::CLIENT))
        {

            CLightEffect::LIGHTEFFECT_DESC Desc;
            LoadjsonEnum(iter,"WorldEvent", Desc.eWorldEventType);
            LoadjsonEnum(iter, "LocalEvent", Desc.eLocalEventType);

            Desc.eHandle.iHandle = LightHandle.iHandle;
            Desc.eHandle.iIndex = LightHandle.iIndex;

            CGameInstance::Get().Add_GameObject_toLayer(iPrototypeLevel, strPrototypeName, iLevel, TEXT("Layer_Light"),&Desc);
        }

   
    }
   
   return S_OK;
}
void CLight_Manager::Light_Dead()
{
    for (uint32_t i = 0; i < ETOUI(USETYPE::END); ++i)
    {
        uint32_t Cnt = 0;
        for (auto iter = m_Lights[i].begin(); iter != m_Lights[i].end();)
        {
            if (i == ETOUI(USETYPE::CLIENT))
            {
                if (NULL_FALSE((*iter)) && (*iter)->Dead_Check())
                {
                    (*iter).reset();
                    m_FreeList.push_back(i);
                    ++m_Generation[Cnt];
                }
                ++Cnt;
            }
            else if (NULL_FALSE((*iter)) && (*iter)->Dead_Check())
            {
                iter = m_Lights[i].erase(iter);
                continue;
            }

            ++iter;
        }
    }
    

}
HRESULT CLight_Manager::Render_Debug_Lights()
{
    for (uint32_t i = 0; i < ETOUI(USETYPE::END); ++i)
    {
        for (auto& pLight : m_Lights[i])
        {
            if(NULL_FALSE(pLight))
            pLight->Debug_Render(m_pDebugShader, m_pDebugVIBuffer);
        }
    }
    
    return S_OK;
}
shared_ptr<CLight> CLight_Manager::Select_Light(_fvector OriginPos, _fvector OriginDir)
{
    for (uint32_t i = 0; i < ETOUI(USETYPE::END); ++i)
    {
        for (auto& Light : m_Lights[i])
        {
            if (NULL_FALSE(Light))
            {
                if (Light->ComParePos(OriginPos, OriginDir))
                    return Light;
            }
        }
    }
    return nullptr;
}
LIGHT_DESC* CLight_Manager::Get_LightToHandle(uint32_t iIndex, uint32_t iHandle)
{
    if (iIndex >= m_Generation.size() || iHandle != m_Generation[iIndex])
        return nullptr;
    
       return m_Lights[ETOUI(USETYPE::CLIENT)][iIndex]->Get_LightDescNoCheckType();
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