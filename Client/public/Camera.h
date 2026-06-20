#pragma once
#include "Client_Defines.h"
#include "CameraNS.h"
#include "Observer.h"

NS_BEGIN(Client)
class CCamera : public CCameraNS
{
public:
    typedef struct tagFreeCamera : public CCameraNS::CAMERA_DESC
    {
        _float4x4* ParentsMatrix;
        _float4x4 CamBoneMatrix;
    }FREECAM_DESC;

private:
    CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCamera(const CCamera& Prototype);

public:
    virtual ~CCamera();

public:
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render() override;

    void            OneMouseMove(_float x, _float y, _float fDeltaTime);
    void            CombinedMatrix(_matrix fmatrix);

    void            Pitch(_float fAngle);
    void            RotationY(_float fAngle);

private:
    INFO                m_pInfo = {};

    _float              m_fAngle = { 0 };
    _float              m_fps = { 0 };
    _float              m_fpsTime = {0};
    _float3             m_fRot = {};
    _float3             m_fRight = {};
    _float3             m_fUp = {};
    _float3             m_fLook = {};

    _float4x4*          m_pParentMatrix = {};
    _float4x4           m_CurrentWorldMatrix = {};
    _float4x4           m_ChildCamBoneMatrix{};
    _float3             m_fLastMousePos = {};
    _float              m_fSpeed = {0};

    _bool               m_bFreecam = {false};
    _bool               m_bMouseMoveCheck = { false };
public:
    static unique_ptr<CCamera> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual shared_ptr<CPrototype> Clone(void* pArg)override;

};

NS_END