#include "Camera.h"
#include "GameInstance.h"
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

    CCamera::CAMERA_DESC		FreeDesc{};
    FreeDesc.vEye = _float4{ 0.f,10.f,-5.f,1.f };
    FreeDesc.vAt = _float4{ 0.f,0.f,0.f,1.f };
    FreeDesc.fFovy = XMConvertToRadians(60.f);
    FreeDesc.fNear = 0.1f;
    FreeDesc.fFar = 1000.f;
    FreeDesc.m_fSpeedPerSec = 100.f;
   
    m_pParentMatrix = pDesc->ParentsMatrix;
    m_ChildCamBoneMatrix = pDesc->CamBoneMatrix;
    m_CurrentWorldMatrix = *m_pParentMatrix;
    if (FAILED(__super::Initialize(&FreeDesc)))
        return E_FAIL;

    m_fLook = { 0.f,0.f,1.f };
    m_fUp = { 0.f,1.f,0.f };
    m_fRight = { 1.f,0.f,0.f };
    m_fSpeed = 2.f;
    
    return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
   
}

void CCamera::Update(_float fTimeDelta)
{
  
}

void CCamera::Late_Update(_float fTimeDelta)
{
    if (CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::F5))
        m_bFreecam = !m_bFreecam;

    if (!m_bFreecam)
    {

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(g_hWnd, &mousePos);
        if (CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::TAB))
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
        OneMouseMove(mousePos.x, mousePos.y, fTimeDelta);

        XMVECTOR fR{}, fU{}, fL{}, fP{};

        fR = XMVector3Normalize(XMLoadFloat3(&m_fRight));
        fU = XMVector3Normalize(XMLoadFloat3(&m_fUp));
        fL = XMVector3Normalize(XMLoadFloat3(&m_fLook));
        fP = m_pTransform->Get_State(STATE::POS);

        fL = XMVector3Normalize(fL);
        fU = XMVector3Normalize(XMVector3Cross(fL, fR));
        fR = XMVector3Cross(fU, fL);

        _float x = -XMVectorGetX(XMVector3Dot(fP, fR));
        _float y = -XMVectorGetX(XMVector3Dot(fP, fU));
        _float z = -XMVectorGetX(XMVector3Dot(fP, fL));

        m_pTransform->Set_State(STATE::RIGHT, fR);
        m_pTransform->Set_State(STATE::UP, fU);
        m_pTransform->Set_State(STATE::LOOK, fL);
    }
    else
    {
        _matrix SrcMatrix = XMLoadFloat4x4(&m_CurrentWorldMatrix);
        _matrix DestMatrix = XMLoadFloat4x4(m_pParentMatrix);

        _matrix FinalMatrix = XMMatrixIdentity();

        _vector SrcRotation = XMQuaternionRotationMatrix(SrcMatrix);
        _vector DestRotaiton = XMQuaternionRotationMatrix(DestMatrix);

        _float fDot = XMVectorGetX(XMQuaternionDot(SrcRotation, DestRotaiton));

        if (fDot < 0.f)
            SrcRotation = -SrcRotation;


        _vector vLastRotation = XMQuaternionSlerp(SrcRotation, DestRotaiton, 1.f - exp(-40.f * fTimeDelta));
        _vector vLastPosition = DestMatrix.r[3];
        FinalMatrix = XMMatrixRotationQuaternion(vLastRotation);
        FinalMatrix.r[3] = vLastPosition;
        CombinedMatrix(FinalMatrix);
        POINT pt;
        pt.x = Client::g_iWinSizeX / 2.f;
        pt.y = Client::g_iWinSizeY / 2.f;

        ClientToScreen(g_hWnd, &pt);

        SetCursorPos(pt.x, pt.y);
    }

    __super::Update_PipeLine();
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
        _float2 ViewPort = CGameInstance::Get().Get_ViewportSize();
        _float dx = XMConvertToRadians( 10.f * fDeltaTime * static_cast<_float>(m_fLastMousePos.x - ViewPort.x/ 2.f));
        _float dy = XMConvertToRadians( 10.f * fDeltaTime * static_cast<_float>(m_fLastMousePos.y - ViewPort.y/2.f  ));

        Pitch(dy);
        RotationY(dx);
    }

    m_fLastMousePos.x = x;
    m_fLastMousePos.y = y;


}
void CCamera::CombinedMatrix(_matrix fmatrix)
{
    XMStoreFloat4x4(&m_CurrentWorldMatrix, fmatrix);

    _matrix OffsetMat = XMMatrixIdentity();
    OffsetMat.r[3] = { 0.f,0.f,0.3f,1.f };

    m_pTransform->Set_Matrix(OffsetMat * XMLoadFloat4x4(&m_CurrentWorldMatrix) * XMLoadFloat4x4(&m_ChildCamBoneMatrix));

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
