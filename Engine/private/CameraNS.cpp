#include "CameraNS.h"
#include "GameInstance.h"
CCameraNS::CCameraNS(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCameraNS::CCameraNS(const CCameraNS& Prototype)
    : CGameObject{ Prototype }
{
}

CCameraNS::~CCameraNS()
{
}

HRESULT CCameraNS::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCameraNS::Initialize(void* pArg)
{
    auto        pDesc = static_cast<CAMERA_DESC*>(pArg);

    CTransform::TRANSFORM_DESC Tr{};
    Tr.m_fRotationPerSec = 10.f;
    Tr.m_fSpeedPerSec = 100.f;
    if (FAILED(__super::Initialize(&Tr)))
        return E_FAIL;

    m_pTransform->Set_State(STATE::POS, XMLoadFloat4(&pDesc->vEye));
    m_pTransform->LookAt(XMLoadFloat4(&pDesc->vAt));

    m_fFovy = pDesc->fFovy;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    Update_PipeLine();

    return S_OK;
}

void CCameraNS::Priority_Update(_float fTimeDelta)
{
}

void CCameraNS::Update(_float fTimeDelta)
{
}

void CCameraNS::Late_Update(_float fTimeDelta)
{
}

HRESULT CCameraNS::Render()
{
    return S_OK;
}

void CCameraNS::Update_PipeLine()
{
    CGameInstance::Get().Set_Transform(D3DTS::VIEW,
        XMMatrixInverse(nullptr, m_pTransform->Get_World()));

    _float2     fViewportSize = CGameInstance::Get().Get_ViewportSize();

    CGameInstance::Get().Set_Transform(D3DTS::PROJ,
        XMMatrixPerspectiveFovLH(m_fFovy, fViewportSize.x / fViewportSize.y,
            m_fNear, m_fFar));
}

