#include "Renderer.h"
#include "GameInstance.h"
#include "ParticleObject.h"
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
    _float2         vViewportSize = CGameInstance::Get().Get_ViewportSize();

    //For Target Diffuse
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Diffuse"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_DiffuseBloom"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    //For Target Normal
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Normal"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    //For Target Shade
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Shade"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    /* For.Target_Specular */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Specular"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Depth"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Fog"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
  
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Emissive"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    //For MRT_GameObject
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    //For MRT_GameObject
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Normal"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_GameObject"), TEXT("Target_Depth"))))
        return E_FAIL;

    //For MRT_LightAcc
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

    //For MRT_Fog
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog"))))
        return E_FAIL;
    
    //For MRT_Bloom
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_BloomBefore"), TEXT("Target_DiffuseBloom"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Emissive"))))
        return E_FAIL;

    m_pVIBuffer = CRect::Create(m_pDevice, m_pContext);
    if(NULL_TRUE(m_pVIBuffer))
        return E_FAIL;
       
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTX_TEX::Elements, VTX_TEX::iNumElements);
    if (NULL_TRUE(m_pShader))
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(vViewportSize.x, vViewportSize.y, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(vViewportSize.x, vViewportSize.y, 0.f, 1.f));

#ifdef _DEBUG
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
       return E_FAIL;
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
       return E_FAIL;
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
       return E_FAIL;
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_Specular"), 450.f, 450.f, 300.f, 300.f)))
       return E_FAIL;
#endif


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
HRESULT CRenderer::Add_RenderToParticle(shared_ptr<class CParticleObject> pObj)
{
    if (NULL_TRUE(pObj))
        return E_FAIL;

    m_pParticleFog = pObj;
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


    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_Lights()))
        return E_FAIL;
    
    if(FAILED(Render_Fog()))
        return E_FAIL;

    if (FAILED(Render_Combined()))
        return E_FAIL;
    
    CGameInstance::Get().Render_Debug_Lights();

    if (FAILED(Render_NonLights()))
        return E_FAIL;

    if (FAILED(Render_BloomBefore()))
        return E_FAIL;
    if (FAILED(Render_Bloom()))
        return E_FAIL;

    if (FAILED(Render_BloomCombine()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

  
    if (FAILED(Render_UI()))
        return E_FAIL;
 
#ifdef _DEBUG
  // if (FAILED(Render_DEBUG()))
  //   return E_FAIL;
#endif


   // CGameInstance::Get().Render_Navi();
    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    
 
   //절두체의 가까운 평면을 계산
   _vector vPlane[6];
  // Culling_Calcurator(vPlane);

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
        {
            ++iRanderCall;
            pRenderObject->Render();
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::PRIORITY)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{

    //절두체의 가까운 평면을 계산
    _vector vPlane[6];
    ////Culling_Calcurator(vPlane); 
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_GameObject"))))
        return E_FAIL;
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
        {
                ++iRanderCall;
                pRenderObject->Render(); 
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::NONBLEND)].clear();
  if (FAILED(CGameInstance::Get().End_MRT()))
      return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pShader->Bind_Matrix("g_ProjMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::PROJ));
    m_pShader->Bind_Matrix("g_ViewMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW));

    m_pShader->Bind_RawValue("g_vCamPosition", CGameInstance::Get().Get_CamPosition(), sizeof _float4);

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Render_Lights(m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;
     
    return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
    auto pObj = m_pParticleFog.lock();
    if (NULL_TRUE(pObj))
        return S_OK;

    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_Fog"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;
    
   
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
    pObj->Bind_Resource("g_fLightAngleRange", m_pShader);

    m_pShader->Bind_Matrix("g_ProjMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::PROJ));
    m_pShader->Bind_Matrix("g_ViewMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW));

    m_pShader->Bind_RawValue("g_vCamPosition", CGameInstance::Get().Get_CamPosition(), sizeof _float4);

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;


    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::FOG))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Fog"), m_pShader, "g_FogTexture")))
        return E_FAIL;
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::COMBINED))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLights()
{

    //m_pContext->PSSetShaderResources(0, 0, nullptr);
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::NONLIGHT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();
    }

    m_RenderObjects[ETOUI(RENDERGROUP::NONLIGHT)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    _vector vPlane[6];
   // Culling_Calcurator(vPlane);
   
    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
        {
           //if (Culling(pRenderObject.get(), vPlane))
           //{
                ++iRanderCall;
                pRenderObject->Render();
           // }
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::BLEND)].clear();
    
    return S_OK;
}

HRESULT CRenderer::Render_BloomBefore()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_BloomBefore"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::BLOOM_BEFORE)])
    {
        if (nullptr != pRenderObject)
        {
            ++iRanderCall;
            pRenderObject->Render();

        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::BLOOM_BEFORE)].clear();
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_Bloom"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ETOUI(RENDERGROUP::BLOOM)])
    {
        if (nullptr != pRenderObject)
        {       
            ++iRanderCall;
            pRenderObject->Render_Bloom();
         
        }
    }

    m_RenderObjects[ETOUI(RENDERGROUP::BLOOM)].clear();
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BloomCombine()
{
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DiffuseBloom", m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_Emissive", m_pShader, "g_EmissiveTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLOOM))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

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

HRESULT CRenderer::Render_DEBUG()
{
    m_pContext->Flush();
    _float4x4   WorldMatrix = {};

    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

   if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_GameObject"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_LightAcc"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;

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

