#include "Target_Manager.h"
#include "RenderTarget.h"
CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
: m_pDevice(pDevice), m_pContext(pContext)
{

}
CTarget_Manager::~CTarget_Manager()
{
}

HRESULT CTarget_Manager::Add_RenderTarger(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelForamt, const _float4& vClearColor)
{
    if (NULL_FALSE(Find_RenderTarget(strTargetTag)))
        return E_FAIL;
    
    auto pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelForamt, vClearColor);
    if (NULL_TRUE(pRenderTarget))
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    auto pRenderTarget = Find_RenderTarget(strTargetTag);
    if (NULL_TRUE(pRenderTarget))
        return E_FAIL;

    auto pMRTList = Find_MRT(strMRTTag);
    if (NULL_TRUE(pMRTList))
    {
        list<shared_ptr<CRenderTarget>> MRTList;

        MRTList.push_back(pRenderTarget);

        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        (*pMRTList).push_back(pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag)
{
    auto        pMRTList = Find_MRT(strMRTTag);
    if (NULL_TRUE(pMRTList))
        return E_FAIL;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    ComPtr<ID3D11RenderTargetView>      RenderTargets[8] = {};

    uint32_t        iNumRenderTargets = { 0 };

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear(); // 랜더 타겟뷰 클리어
        RenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV(); //현재 배열에 담겨있던 랜더 타겟 클래스의 랜더 타겟정보 가져오기
    }

                                //랜더 타겟 몇개      가져온 랜더타겟 전부             깊이 스탠실 버퍼 기존꺼 사용한다고                      
    m_pContext->OMSetRenderTargets(iNumRenderTargets, RenderTargets[0].GetAddressOf(), m_pOriginalDSV.Get());
    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    ComPtr<ID3D11RenderTargetView>          RenderTargets[8] = { m_pBackBufferRTV };
    m_pContext->OMSetRenderTargets(8, RenderTargets[0].GetAddressOf(), m_pOriginalDSV.Get());
    return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, shared_ptr<CShader> pShader, const _char* pConstantName)
{
    auto pRenderTarget = Find_RenderTarget(strTargetTag);
    if (NULL_TRUE(pRenderTarget))
        return E_FAIL;

    return pRenderTarget->Bind_ShaderResource(pShader,pConstantName);
}

HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float SizeX, _float SizeY)
{
    auto pRenderTarget = Find_RenderTarget(strTargetTag);
    if (NULL_TRUE(pRenderTarget))
        return E_FAIL;


    return pRenderTarget->Ready_Debug(fX,fY,SizeX,SizeY);
}

HRESULT CTarget_Manager::Debug_Render(const _wstring& strMRTTag, shared_ptr<CShader> pShader, const _char* pConstantName, shared_ptr<CRect> pVIBuffer)
{
    auto    pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        if (nullptr != pRenderTarget)
            pRenderTarget->Debug_Render(pShader, pConstantName, pVIBuffer);
    }

    return S_OK;
}

shared_ptr<CRenderTarget> CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
    auto    iter = m_RenderTargets.find(strTargetTag);

    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<shared_ptr<CRenderTarget>>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto    iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}
unique_ptr<CTarget_Manager> CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return unique_ptr<CTarget_Manager>(new CTarget_Manager(pDevice, pContext));
}
