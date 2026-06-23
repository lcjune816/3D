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
    m_fViewPortSize = vViewportSize;
    //For Target Diffuse
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Diffuse"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_DiffuseBloom"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    //For Target Normal
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Normal"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_NormalBloom"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    //For Target Shade
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Shade"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    /* For.Target_Specular */
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Specular"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Depth"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_DepthBloom"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Fog"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
  
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Emissive"), vViewportSize.x, vViewportSize.y, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurHorizon"), vViewportSize.x / m_iSampleing[1], vViewportSize.y / m_iSampleing[1], DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_BlurVertical"), vViewportSize.x / m_iSampleing[1], vViewportSize.y / m_iSampleing[1], DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    //For.Target Sampler

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_DownSampleTwo"), vViewportSize.x / m_iSampleing[0], vViewportSize.y/ m_iSampleing[0], DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_DownSampleFour"), vViewportSize.x/ m_iSampleing[1], vViewportSize.y/ m_iSampleing[1], DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_UpSampleTwo"), vViewportSize.x , vViewportSize.y , DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_UpSampleFour"), vViewportSize.x / m_iSampleing[0], vViewportSize.y / m_iSampleing[0], DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0, 0, 0, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_RenderTarget(TEXT("Target_Scene"), vViewportSize.x , vViewportSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0, 0, 0, 1.f))))
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
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_DownSample2"), TEXT("Target_DownSampleTwo"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_DownSample4"), TEXT("Target_DownSampleFour"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_UpSample2"), TEXT("Target_UpSampleTwo"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_UpSample4"), TEXT("Target_UpSampleFour"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Emissive"), TEXT("Target_Emissive"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Emissive"), TEXT("Target_NormalBloom"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Emissive"), TEXT("Target_DepthBloom"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
        return E_FAIL;

    //For MRT_Blur
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_BlurHorizontal"), TEXT("Target_BlurHorizon"))))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Add_MRT(TEXT("MRT_BlurVertical"), TEXT("Target_BlurVertical"))))
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
  if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_DownSampleTwo"), 150.f, 150.f, 300.f, 300.f)))
      return E_FAIL;
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_DownSampleFour"), 450.f, 150.f, 300.f, 300.f)))
       return E_FAIL;
   if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_UpSampleTwo"), 150.f, 450.f, 300.f, 300.f)))
       return E_FAIL;
  if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_UpSampleFour"), 450.f, 450.f, 300.f, 300.f)))
      return E_FAIL;
 // if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_UpSampleFour"), 150.f, 450.f, 300.f, 300.f)))
 //     return E_FAIL;
 // if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_BlurHorizon"), 150.f, 720.f, 300.f, 300.f)))
 //     return E_FAIL;
 // if (FAILED(CGameInstance::Get().Ready_RT_Debug(TEXT("Target_BlurVertical"), 450.f, 720.f, 300.f, 300.f)))
 //     return E_FAIL;
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

HRESULT CRenderer::Draw()
{
    iRanderCall = 0;

    if (CGameInstance::Get().Get_DIKeyState(DIK_0) & 0x80)
        iDebuhRender = 0;

    if (CGameInstance::Get().Get_DIKeyState(DIK_1) & 0x80)
        iDebuhRender = 1;
  
    if (CGameInstance::Get().Get_DIKeyState(DIK_2) & 0x80)
        iDebuhRender = 2;
    if (CGameInstance::Get().Get_DIKeyState(DIK_3) & 0x80)
        iDebuhRender = 3;
    if (CGameInstance::Get().Get_DIKeyState(DIK_4) & 0x80)
        iDebuhRender = 4;
    if (CGameInstance::Get().Get_DIKeyState(DIK_5) & 0x80)
        iDebuhRender = 5;
    if (CGameInstance::Get().Get_DIKeyState(DIK_6) & 0x80)
        iDebuhRender = 6;
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

    if (FAILED(Render_Emissive()))
        return E_FAIL;

    //if (FAILED(Render_LightsBloom()))
    //    return E_FAIL;

    if (FAILED(Render_DownSample())) 
        return E_FAIL;

    if (FAILED(Render_DownSampleBlur())) 
        return E_FAIL;

    if (FAILED(Render_UpSampleBlur())) 
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

void CRenderer::SetUp_ViewPort(uint32_t iWidth, uint32_t iHeight)
{
    D3D11_VIEWPORT          ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof D3D11_VIEWPORT);
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = (_float)iWidth;
    ViewPortDesc.Height = (_float)iHeight;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;
    m_pContext->RSSetViewports(1, &ViewPortDesc);
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

HRESULT CRenderer::Render_LightsBloom()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_NormalBloom"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_DepthBloom"), m_pShader, "g_DepthTexture")))
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
    if ( FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_Scene"))))
        return E_FAIL;

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

    if (FAILED(CGameInstance::Get().End_MRT()))
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

HRESULT CRenderer::Render_Emissive()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_Emissive"))))
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

HRESULT CRenderer::Render_BlurHorizontal()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_BlurHorizontal"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_Emissive"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    m_pShader->Bind_Matrix("g_ProjMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::PROJ));
    m_pShader->Bind_Matrix("g_ViewMatrixInverse", CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW));


    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLURHORIZON))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;
    return S_OK;
}

HRESULT CRenderer::Render_BlurVertical()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_BlurVertical"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_BlurHorizon"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;


    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLURVERTICAL))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_DownSample()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(L"MRT_DownSample2"))) 
        return E_FAIL;

    SetUp_ViewPort(m_fViewPortSize.x / m_iSampleing[0], m_fViewPortSize.y / m_iSampleing[0]);

    _float2 vViewPort = _float2(m_fViewPortSize.x / m_iSampleing[0], m_fViewPortSize.y / m_iSampleing[0]);
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_Emissive", m_pShader, "g_EmissiveTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::DOWNSAMPLE2))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;



    if (FAILED(CGameInstance::Get().Begin_MRT(L"MRT_DownSample4"))) 
        return E_FAIL;

    vViewPort = _float2(m_fViewPortSize.x / m_iSampleing[1], m_fViewPortSize.y / m_iSampleing[1]);
    m_pShader->Bind_RawValue("g_ViewPortSize", &vViewPort, sizeof vViewPort);
    SetUp_ViewPort(m_fViewPortSize.x / m_iSampleing[1], m_fViewPortSize.y / m_iSampleing[1]);
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DownSampleTwo", m_pShader, "g_DownSample")))
        return E_FAIL;
    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::DOWNSAMPLE4))))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().End_MRT())) 
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_DownSampleBlur()
{
    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_BlurHorizontal"))))
        return E_FAIL;

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_DownSampleFour"), m_pShader, "g_BlurTexture")))
        return E_FAIL;

    SetUp_ViewPort(m_fViewPortSize.x / m_iSampleing[1], m_fViewPortSize.y / m_iSampleing[1]);
    _float2 vViewPort = _float2(m_fViewPortSize.x / m_iSampleing[1], m_fViewPortSize.y / m_iSampleing[1]);
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
    m_pShader->Bind_RawValue("g_ViewPortSize", &vViewPort, sizeof vViewPort);
    

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLURHORIZON))))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;



    if (FAILED(CGameInstance::Get().Begin_MRT(TEXT("MRT_BlurVertical"))))
        return E_FAIL;

    SetUp_ViewPort(m_fViewPortSize.x / m_iSampleing[1], m_fViewPortSize.y / m_iSampleing[1]);
    m_pShader->Bind_RawValue("g_ViewPortSize", &vViewPort, sizeof vViewPort);

    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(TEXT("Target_BlurHorizon"), m_pShader, "g_BlurTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::BLURVERTICAL))))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_UpSampleBlur()
{

    if (FAILED(CGameInstance::Get().Begin_MRT(L"MRT_UpSample4"))) 
        return E_FAIL;

    SetUp_ViewPort(m_fViewPortSize.x / m_iSampleing[0], m_fViewPortSize.y / m_iSampleing[0]);
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DownSampleFour", m_pShader, "g_DownSample")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_BlurVertical", m_pShader, "g_BlurTexture")))
        return E_FAIL;

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::UPSAMPLE4))))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().End_MRT()))
        return E_FAIL;


    if (FAILED(CGameInstance::Get().Begin_MRT(L"MRT_UpSample2"))) 
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_UpSampleFour", m_pShader, "g_BlurTexture")))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DownSampleTwo", m_pShader, "g_DownSample")))
        return E_FAIL;
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(DEFERRED::UPSAMPLE2))))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;
    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;
    if (FAILED(CGameInstance::Get().End_MRT())) return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BloomCombine()
{
    DEFERRED eType{};

    if (iDebuhRender == 0)
    {

        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_Scene", m_pShader, "g_SceneTexture")))
            return E_FAIL;
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DiffuseBloom", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_UpSampleTwo", m_pShader, "g_DownSample")))
            return E_FAIL;

        eType = DEFERRED::BLOOM;
    }
    else if (iDebuhRender == 1)
    {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DownSampleTwo", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;

        eType = DEFERRED::HELP;
    }
    else if (iDebuhRender == 2)
    {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_DownSampleFour", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;

        eType = DEFERRED::HELP;
    }
    else if (iDebuhRender == 3)
    {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_BlurHorizon", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;

        eType = DEFERRED::HELP;
    }
    else if (iDebuhRender == 4)
    {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_BlurVertical", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;
       

        eType = DEFERRED::HELP;
    }
    else if (iDebuhRender == 5)
    {
        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_UpSampleFour", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;

        eType = DEFERRED::HELP;
    }
    else if (iDebuhRender == 6)
    {
        

        if (FAILED(CGameInstance::Get().Bind_RT_ShaderResource(L"Target_UpSampleTwo", m_pShader, "g_DiffuseTexture")))
            return E_FAIL;

        eType = DEFERRED::HELP;
    }

    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    if (FAILED(m_pShader->Begin(ETOUI(eType))))
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

   //if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_GameObject"), m_pShader, "g_Texture", m_pVIBuffer)))
   //     return E_FAIL;
   if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_DownSample2"), m_pShader, "g_Texture", m_pVIBuffer)))
       return E_FAIL;
    if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_DownSample4"), m_pShader, "g_Texture", m_pVIBuffer)))
        return E_FAIL;

 //  if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_Emissive"), m_pShader, "g_Texture", m_pVIBuffer)))
 //      return E_FAIL;
   if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_UpSample2"), m_pShader, "g_Texture", m_pVIBuffer)))
       return E_FAIL;
   
   if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_UpSample4"), m_pShader, "g_Texture", m_pVIBuffer)))
       return E_FAIL;
   //
   //if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_BlurHorizontal"), m_pShader, "g_Texture", m_pVIBuffer)))
   //    return E_FAIL;
   //
   //if (FAILED(CGameInstance::Get().Debug_RT_Render(TEXT("MRT_BlurVertical"), m_pShader, "g_Texture", m_pVIBuffer)))
   //    return E_FAIL;
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

