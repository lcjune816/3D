#include "Camera.h"
CCamera::CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
: CCameraNS(pDevice, pContext)
{

}
CCamera::CCamera(const CCamera& Prototype): CCameraNS(Prototype)
{

}

CCamera::~CCamera()
{
}

HRESULT CCamera::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    auto    pDesc = static_cast<FREECAM_DESC*>(pArg);

    pDesc->m_fSpeedPerSec = 100.f;
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _float fFov(0), fAspect(0), fNear(0), fFar(0);
    

    m_fLook = { 0.f,0.f,1.f };
    m_fUp = { 0.f,1.f,0.f };
    m_fRight = { 1.f,0.f,0.f };
    m_fSpeed = 2.f;

    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
   
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(g_hWnd, &mousePos);
    if (GetKeyState(VK_TAB) & 0x8000)
         m_bMouseMoveCheck = !m_bMouseMoveCheck;
  
    if (m_bMouseMoveCheck)
    {
        POINT pt;
        pt.x = Client::g_iWinSizeX / 2.f;
        pt.y = Client::g_iWinSizeY / 2.f;

        ClientToScreen(g_hWnd, &pt);

        SetCursorPos(pt.x, pt.y);
    }

    XMVECTOR vSpeed{}, vLook{}, vPos{};
    if (GetKeyState('W') & 0x8000)
    {
        m_pTransform->Go_Straight(fTimeDelta);
    }
    if (GetKeyState('S') & 0x8000)
    {
        m_pTransform->Go_BackWard(fTimeDelta);
    }
    if (GetKeyState('D') & 0x8000)
    {
        m_pTransform->Go_Right(fTimeDelta);
    }
    if (GetKeyState('A') & 0x8000)
    {
        m_pTransform->Go_Left(fTimeDelta);
    }
     //회전
    OneMouseMove(mousePos.x, mousePos.y,fTimeDelta);

    XMVECTOR fR{}, fU{}, fL{}, fP{};

    fR = XMLoadFloat3(&m_fRight);
    fU = XMLoadFloat3(&m_fUp);
    fL = XMLoadFloat3(&m_fLook);
    fP = m_pTransform->Get_State(STATE::POS);

    fL = XMVector3Normalize(fL);
    fU = XMVector3Normalize(XMVector3Cross(fL, fR));
    fR = XMVector3Cross(fU, fL);

    _float x = -XMVectorGetX(XMVector3Dot(fP, fR));
    _float y = -XMVectorGetX(XMVector3Dot(fP, fU));
    _float z = -XMVectorGetX(XMVector3Dot(fP, fL));

    m_pTransform->Set_State(STATE::RIGHT,  fR);
    m_pTransform->Set_State(STATE::UP    , fU);
    m_pTransform->Set_State(STATE::LOOK  , fL);
  
    __super::Update_PipeLine();

}

void CCamera::Update(_float fTimeDelta)
{

  

}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}
void CCamera::OneMouseMove(_float x, _float y,_float fDeltaTime)
{

    auto& io = ImGui::GetIO(); //마우스 ui에서 사용중이면 리턴
    if (io.WantCaptureMouse)
        return;


    if (m_bMouseMoveCheck)
    {

        _float dx = XMConvertToRadians( 10.f * fDeltaTime * static_cast<_float>(m_fLastMousePos.x - 1280.f / 2.f));
        _float dy = XMConvertToRadians( 10.f * fDeltaTime * static_cast<_float>(m_fLastMousePos.y - 720.f/2.f  ));

        Pitch(dy);
        RotationY(dx);
    }

    m_fLastMousePos.x = x;
    m_fLastMousePos.y = y;


}
void CCamera::Pitch(_float fAngle)
{
    XMMATRIX matRot =  XMMatrixRotationAxis(XMLoadFloat3(&m_fRight) ,fAngle);
    XMStoreFloat3(&m_fUp, XMVector3TransformNormal(XMLoadFloat3(&m_fUp), matRot));
    XMStoreFloat3(&m_fLook, XMVector3TransformNormal(XMLoadFloat3(&m_fLook), matRot));

}
void CCamera::RotationY(_float fAngle)
{
    XMMATRIX matRot2 = XMMatrixRotationY(fAngle);

    XMStoreFloat3(&m_fRight, XMVector3TransformNormal(XMLoadFloat3(&m_fRight), matRot2));
    XMStoreFloat3(&m_fUp, XMVector3TransformNormal(XMLoadFloat3(&m_fUp), matRot2));
    XMStoreFloat3(&m_fLook, XMVector3TransformNormal(XMLoadFloat3(&m_fLook), matRot2));

}
unique_ptr<CCamera>  CCamera::Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext>pContext)
{
    auto pInstance = unique_ptr<CCamera>(new CCamera(pDevice,pContext));
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed CCamera");
        return nullptr;
    }
    return pInstance;
}
shared_ptr<CPrototype> CCamera::Clone(void* pArg)
{
    auto pInstance = shared_ptr<CCamera>(new CCamera(*this));
    if (FAILED(pInstance->Initialize(pArg)))
    {   
        MSG_BOX("Create Failed CCamera_Clone");
        return nullptr;
    }
       return pInstance;
}
