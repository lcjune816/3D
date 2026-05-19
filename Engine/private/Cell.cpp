#include "Cell.h"
#include "GameInstance.h"




CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

CCell::~CCell()
{

}

HRESULT CCell::Initialize(const _float3* pPoints, int32_t index)
{
    m_NaviInfo.iIndex = index;
    memcpy(m_NaviInfo.vPoints, pPoints, sizeof(_float3) * ETOUI(EPOINT::END));

    _vector vLine{};
    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]);
    m_NaviInfo.vNormals[ETOUI(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]);
    m_NaviInfo.vNormals[ETOUI(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]);
    m_NaviInfo.vNormals[ETOUI(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    for (size_t i = 0; i < ETOUI(LINE::END); ++i)
    {
        XMStoreFloat3(&m_NaviInfo.vNormals[i],
            XMVector3Normalize(XMLoadFloat3(&m_NaviInfo.vNormals[i])));
        m_NaviInfo.vPoints[i].y = 0.5f;
    }

  
    XMStoreFloat3(&m_NaviInfo.vCenter, (XMLoadFloat3(&m_NaviInfo.vPoints[0]) + XMLoadFloat3(&m_NaviInfo.vPoints[1])
        + XMLoadFloat3(&m_NaviInfo.vPoints[2])) * 0.3f);
#ifdef _DEBUG
    CVIBuffer::BUFFER_DESC pDesc;
    pDesc.fPos[0] = m_NaviInfo.vPoints[0];
    pDesc.fPos[1] = m_NaviInfo.vPoints[1];
    pDesc.fPos[2] = m_NaviInfo.vPoints[2];

    m_pVIBuffer = static_pointer_cast<CTriAngle>(CGameInstance::Get().Clone_Prototype(0, L"OBJ_Triangle", &pDesc));
    
   if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif
    return S_OK;
}
json CCell::Save_Data()
{
    nlohmann::json j;
    for (int32_t i = 0; i < 3; ++i)
    {
        j["Pos"][i] = { m_NaviInfo.vPoints[i].x,m_NaviInfo.vPoints[i].y,m_NaviInfo.vPoints[i].z };

    }
    return j;
}
HRESULT CCell::Render(CShader* pShader)
{
    _float4 fColor{ 1,1,1,1 };
    if (m_bChoice)
        fColor = { 0.3f,0.7f,0.4f,1.f };
    else
        fColor = { 1,1,1,1.f };
       
    if (m_bChoice)
    {
        pShader->Bind_RawValue("g_Color", &fColor, sizeof _float4);

        pShader->Begin(0);
    }
        
    m_pVIBuffer->Bind_Resource();

    m_pVIBuffer->Render();
    if (m_bChoice)
    {
        fColor = { 1,1,1,1 };
        pShader->Bind_RawValue("g_Color", &fColor, sizeof _float4);

        pShader->Begin(0);
    }

    return S_OK;
}
shared_ptr<CCell> CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, int32_t iIndex)
{
    auto		pInstance = shared_ptr<CCell>(new CCell(pDevice, pContext));

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        return nullptr;
    }

    return pInstance;
}