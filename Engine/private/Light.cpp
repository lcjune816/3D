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
    m_LightOrigin = LightDesc;
    m_iIndex ;
    return S_OK;
}

HRESULT CLight::Initialize_Load(json& j)
{
    if (NULL_TRUE(j))
        return E_FAIL;


    //ENUM
    LoadjsonEnum(j, "LightType", m_LightDesc.eType);
    LoadjsonEnum(j, "UseType", m_LightDesc.eUseType);
    LoadjsonEnum(j, "WorldEvent", m_LightDesc.eWorldEventType);
    LoadjsonEnum(j, "LocalEvent", m_LightDesc.eLocalEventType);


    //FLOAT
    JsonSaveLoadManager::LoadJsonTypeFloat(j, "LightAngleRagne", m_LightDesc.fAngleRange);
    
    //FLOAT2
    JsonSaveLoadManager::LoadJsonTypeFloat2(j, "LightRangeF2", m_LightDesc.fRange);

    //FLOAT4
    JsonSaveLoadManager::LoadJsonTypeFloat4(j, "LightAmbient", m_LightDesc.vAmbient);
    JsonSaveLoadManager::LoadJsonTypeFloat4(j, "LightDiffuse", m_LightDesc.vDiffuse);
    JsonSaveLoadManager::LoadJsonTypeFloat4(j, "LightDir", m_LightDesc.vDir);
    JsonSaveLoadManager::LoadJsonTypeFloat4(j, "LightPos", m_LightDesc.vPos);
    JsonSaveLoadManager::LoadJsonTypeFloat4(j, "LightSpecular", m_LightDesc.vSpecular);
    m_LightOrigin = m_LightDesc;
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
    else if(LIGHT::POINT == m_LightDesc.eType)
    {
        if(FAILED(pShader->Bind_RawValue("g_vLightPos",&m_LightDesc.vPos, sizeof m_LightDesc.vPos)))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof m_LightDesc.fRange)))
            return E_FAIL;

        iPassIndex = ETOUI(DEFERRED::POINT);
    }
    else
    {
        if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof m_LightDesc.vPos)))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof m_LightDesc.vDir)))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof m_LightDesc.fRange)))
            return E_FAIL;
        if (FAILED(pShader->Bind_RawValue("g_fLightAngleRange", &m_LightDesc.fAngleRange, sizeof m_LightDesc.fAngleRange)))
            return E_FAIL;

        iPassIndex = ETOUI(DEFERRED::SPOTLIGHT);

    }

    if(FAILED(pShader->Bind_RawValue("g_vLightDiffuse",&m_LightDesc.vDiffuse, sizeof m_LightDesc.vDiffuse)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof m_LightDesc.vAmbient)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof m_LightDesc.vSpecular)))
        return E_FAIL;

    if (FAILED(pShader->Begin(iPassIndex)))
        return E_FAIL;

    if (FAILED(pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

json			CLight::Save_Data()
{
    json j{};
    SavejsonEnum(j, "LightType", m_LightOrigin.eType);
    SavejsonEnum(j, "UseType", m_LightOrigin.eUseType);
    SavejsonEnum(j, "WorldEvent", m_LightOrigin.eWorldEventType);
    SavejsonEnum(j, "LocalEvent", m_LightOrigin.eLocalEventType);


    j["LightAngleRagne"] = m_LightOrigin.fAngleRange;
    JsonSaveLoadManager::SaveJsonTypeFloat2(j,"LightRangeF2", m_LightOrigin.fRange);
    JsonSaveLoadManager::SaveJsonTypeFloat4(j,"LightAmbient", m_LightOrigin.vAmbient);
    JsonSaveLoadManager::SaveJsonTypeFloat4(j,"LightDiffuse", m_LightOrigin.vDiffuse);
    JsonSaveLoadManager::SaveJsonTypeFloat4(j, "LightDir",    m_LightOrigin.vDir);
    JsonSaveLoadManager::SaveJsonTypeFloat4(j, "LightPos",    m_LightOrigin.vPos);
    JsonSaveLoadManager::SaveJsonTypeFloat4(j, "LightSpecular", m_LightOrigin.vSpecular);

    return j;

}
HRESULT CLight::Debug_Render(shared_ptr<class CShader> pShader, shared_ptr<class CCube> pViBuffer)
{
    _float4x4 WorldMatrix{};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    memcpy(&WorldMatrix.m[3], reinterpret_cast<_float*>(&m_LightDesc.vPos), sizeof _float4);
    pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
    pShader->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
    pShader->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

    pShader->Bind_RawValue("g_Color", &m_fColor, sizeof _float4);

    pShader->Begin(0);

    pViBuffer->Bind_Resource();
    pViBuffer->Render();
    return S_OK;
}

_bool CLight::ComParePos(_fvector vOriginPos, _fvector vOriginDir)
{
    _vector SrcPos = XMLoadFloat4(&m_LightDesc.vPos);

    BoundingBox box;
    box.Center = _float3(m_LightDesc.vPos.x, m_LightDesc.vPos.y, m_LightDesc.vPos.z);
    box.Extents = _float3(0.5f, 0.5f, 0.5f);

    _float fDis{};
    if (box.Intersects(vOriginPos, vOriginDir, fDis))
    {
        m_fColor = { 1.f,0.f,0.f,0.f };
        return true;
    }
    m_fColor = { 1.f,1.f,1.f,1.f };
    return false;
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

shared_ptr<CLight> CLight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, json& LoadJson)
{
    auto pInstance = shared_ptr<CLight>(new CLight(pDevice, pContext));
    if (FAILED(pInstance->Initialize_Load(LoadJson)))
    {
        MSG_BOX("Create Failed Light");
        return nullptr;
    }
    return pInstance;
}
