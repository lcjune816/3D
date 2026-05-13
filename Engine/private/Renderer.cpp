#include "Renderer.h"
#include "GameInstance.h"
CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

CRenderer::~CRenderer()
{
}

HRESULT CRenderer::Initialize()
{
    return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pRenderObject)
{
    if (eRenderGroup >= RENDERGROUP::END ||
        nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObjects[ETOUI(eRenderGroup)].push_back(pRenderObject);

    return S_OK;
}
_bool CRenderer::Culling(CGameObject* pObj, _vector* vec)
{
    //벽느끼네
  
    _matrix matrix = pObj->Get_Transform().lock()->Get_World();
    _vector vPos{};
    _float3 fMax = pObj->Get_Transform().lock()->Get_Max();
    _float3 fMin = pObj->Get_Transform().lock()->Get_Min();
    _vector vCenter =  ((XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f);

    _float vRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&fMax) - vCenter));

    vCenter = XMVector3TransformCoord(vCenter, matrix);
    _vector fx = matrix.r[0];
    _vector fy = matrix.r[1];
    _vector fz = matrix.r[2];
    _float fScale = max(XMVectorGetX(XMVector3Length(fz)),max(XMVectorGetX(XMVector3Length(fx)), XMVectorGetX(XMVector3Length(fy))));
    vRadius = vRadius * fScale;
   
    for (int32_t i = 0; i < 6; ++i)
    {
        _float fDot = XMVectorGetX(XMPlaneDot(vec[i], XMVectorSetW(vCenter,1.f)));

        if (fDot  < -vRadius)
            return false;
    }
  
    return true;
}
void CRenderer::Culling_Calcurator(_fvector* vP)
{
    _float4x4 matProj = *CGameInstance::Get().Get_Transform(D3DTS::PROJ);
    _float4x4 matView = *CGameInstance::Get().Get_Transform(D3DTS::VIEW);

    _float zMinimum = -matProj._43 / matProj._33; // 절두체 최소 z거리 계산
    _float r = 500.f / (500.f - zMinimum);

    matProj._33 = r;
    matProj._43 = -r * zMinimum;
    _matrix  FinalMatrix = XMMatrixMultiply(XMLoadFloat4x4(&matView), XMLoadFloat4x4(&matProj));
    _float4x4 fMatrix = {};
    XMStoreFloat4x4(&fMatrix, FinalMatrix);
    //절두체의 가까운 평면을 계산
    _vector vPlane[6];
    _float x = fMatrix._14 + fMatrix._13;
    _float y = fMatrix._24 + fMatrix._23;
    _float z = fMatrix._34 + fMatrix._33;
    _float w = fMatrix._44 + fMatrix._43;
    vPlane[0] = XMVectorSet(x, y, z, w);

    //먼평면
    x = fMatrix._14 - fMatrix._13;
    y = fMatrix._24 - fMatrix._23;
    z = fMatrix._34 - fMatrix._33;
    w = fMatrix._44 - fMatrix._43;
    vPlane[1] = XMVectorSet(x, y, z, w);

    //왼쪽평면
    x = fMatrix._14 + fMatrix._11;
    y = fMatrix._24 + fMatrix._21;
    z = fMatrix._34 + fMatrix._31;
    w = fMatrix._44 + fMatrix._41;
    vPlane[2] = XMVectorSet(x, y, z, w);
    // 오른쪽 평면
    x = fMatrix._14 - fMatrix._11;
    y = fMatrix._24 - fMatrix._21;
    z = fMatrix._34 - fMatrix._31;
    w = fMatrix._44 - fMatrix._41;
    vPlane[3] = XMVectorSet(x, y, z, w);

    //윗평면
    x = fMatrix._14 - fMatrix._12;
    y = fMatrix._24 - fMatrix._22;
    z = fMatrix._34 - fMatrix._32;
    w = fMatrix._44 - fMatrix._42;
    vPlane[4] = XMVectorSet(x, y, z, w);

    //아래 평면
    x = fMatrix._14 + fMatrix._12;
    y = fMatrix._24 + fMatrix._22;
    z = fMatrix._34 + fMatrix._32;
    w = fMatrix._44 + fMatrix._42;
    vPlane[5] = XMVectorSet(x, y, z, w);

    for (int32_t i = 0; i < 6; ++i)
    {
        vPlane[i] = XMPlaneNormalize(vPlane[i]);
    }

    memcpy((_vector*)(&(*vP)), &vPlane, sizeof(_vector)*6);
}
HRESULT CRenderer::Draw()
{
    iRanderCall = 0;

//    CGameInstance::Get().Draw_Instancing();
    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    
 
   //절두체의 가까운 평면을 계산
   _vector vPlane[6];
   Culling_Calcurator(vPlane);

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
        {
           if (Culling(pRenderObject.get(), vPlane))
           {
                ++iRanderCall;
                pRenderObject->Render();
           }
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{

    //절두체의 가까운 평면을 계산
    _vector vPlane[6];
    Culling_Calcurator(vPlane);

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
        {
            if (Culling(pRenderObject.get(), vPlane))
            {
                ++iRanderCall;

                pRenderObject->Render();
            }
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    _vector vPlane[6];
    Culling_Calcurator(vPlane);

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
        {
            if (Culling(pRenderObject.get(), vPlane))
            {
                ++iRanderCall;
                pRenderObject->Render();
            }
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::BLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::UI)])
    {
        if (nullptr != pRenderObject)
        {
            pRenderObject->Render();
            ++iRanderCall;
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::UI)].clear();

    return S_OK;
}

unique_ptr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = unique_ptr<CRenderer>(new CRenderer(pDevice, pContext));

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        return nullptr;
    }

    return pInstance;
}

