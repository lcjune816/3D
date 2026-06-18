#include "Navigation.h"
#include "Layer.h"
#include "GameInstance.h"
CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CNavigation::~CNavigation()
{
}
HRESULT CNavigation::Initialize_Prototype(const _wstring& FilePath, const _char* pName)
{
   // Make_NaviToTerrain();
   Load_Navi(FilePath, pName);
#ifdef _DEBUG

   m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTX_POS::Elements, VTX_POS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

}
HRESULT CNavigation::Initialize(void* pArg)
{
    if (NULL_TRUE(pArg))
        return S_OK;

    auto pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    m_eOwner = pDesc->eOwner;
    m_iCurretnCellindex = pDesc->iIndex;

    return S_OK;

}
HRESULT CNavigation::Ready_Neightbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell )
                continue;

            if (true == pDestCell->Compare_Points(pSourCell->Get_CellPos(ETOUI(EPOINT::B)), pSourCell->Get_CellPos(ETOUI(EPOINT::A))))
            {
                pSourCell->Set_Neighbor(LINE::AB, pDestCell);
            }
            if (true == pDestCell->Compare_Points(pSourCell->Get_CellPos(ETOUI(EPOINT::C)), pSourCell->Get_CellPos(ETOUI(EPOINT::B))))
            {
                pSourCell->Set_Neighbor(LINE::BC, pDestCell);
            }
            if (true == pDestCell->Compare_Points(pSourCell->Get_CellPos(ETOUI(EPOINT::A)), pSourCell->Get_CellPos(ETOUI(EPOINT::C))))
            {
                pSourCell->Set_Neighbor(LINE::CA, pDestCell);
            }

        }
    }

    return S_OK;
}

_bool CNavigation::InMove(_fvector vResultPos,_float3* fDir)
{
    //더이상 갈수있는 노드가 없을경우

    if (-1 == m_iCurretnCellindex || m_Cells.empty())
        return false;
    
    if (m_eOwner == OWNER::PLAYER && m_iPreCellIndex != m_iCurretnCellindex)
    {
        Event_Check(m_Cells[m_iCurretnCellindex]->Get_Event());
        m_iPreCellIndex = m_iCurretnCellindex;
    }

    _bool       bFinished{ false };
    int32_t     iNeighborIndex = { -1 };
    //인접한 노드가있고 해당 위치에 오브젝트가 위치할경우
    if (true == m_Cells[m_iCurretnCellindex]->IsIn(vResultPos, &iNeighborIndex, fDir))
    {
        return true;
    }
    else
    {
        //나간 방향에 이웃이 존재할경우
        if (-1 != iNeighborIndex)
        {
            while (true)
            {
                //진짜 그 위치에 있는지 재탐색
                if (true == m_Cells[iNeighborIndex]->IsIn(vResultPos, &iNeighborIndex, fDir))
                {
                    break;
                }
                
                if (-1 == iNeighborIndex)
                    return false;
            }

            m_iCurretnCellindex = iNeighborIndex;
            return true;
        }
        else
            return false;

    }
}
_vector CNavigation::SetUp_OnNavigation(_fvector vPos, _float offsetY)
{
    if (m_Cells.empty() || m_iCurretnCellindex == -1.f || (m_eOwner == OWNER::PLAYER) && (m_eEvent == CELL_EVENT::ELEVATOR))
        return vPos;

    return XMVectorSetY(vPos, m_Cells[m_iCurretnCellindex]->Compute_Height(vPos) + offsetY);
    
}
_bool CNavigation::AStartAlgorithm(const uint32_t endLayerIndex,  const _wstring& LayerName, const _char* tagName, _fvector SrcPos,_float3 *fPos)
{
    if (m_Cells.empty())
        return false;
    if (-1 == m_iCurretnCellindex)
        return false;

    auto pObj = Find_Object(endLayerIndex, LayerName, tagName);
    if (NULL_TRUE(pObj))
        return false;


    auto pDestNavi = static_pointer_cast<CNavigation>(pObj->Find_Component(L"Com_Navigation"));
    if (NULL_TRUE(pDestNavi))
        return false;
    int32_t index = { -1 };
    _vector LastPos = pDestNavi->Get_CurrentCell_Info(&index);
    if (-1 == index)
        return false;
    if (m_iCurretnCellindex == index )
    {
        XMStoreFloat3(fPos, LastPos);
        return true;
    }
    XMStoreFloat3(fPos ,XMVectorSet(0,0,0,0));
     _vector DestPos = pObj->Get_Transform().lock()->Get_State(STATE::POS);

     m_AstarCloseList.clear();
     m_AstarOpenList.clear();
     m_MoveToList.clear();
     _float Huritices = XMVectorGetX(XMVector3Length(DestPos - SrcPos));
     ENGINE_ASTAR FirstAstar{};
     
     FirstAstar.H = Huritices;
     FirstAstar.G = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[m_iCurretnCellindex]->Get_NaviInfo().vCenter) - SrcPos));
     FirstAstar.F = FirstAstar.G + FirstAstar.H;
     FirstAstar.iParent_node = -1;
     FirstAstar.iNode_Nubmer = m_iCurretnCellindex;
     m_AstarCloseList.push_back(FirstAstar);

     while (true)
     {
         _bool bFinished = false;
         if (bFinished)
             return true;

         if (!m_AstarOpenList.empty())
         {
             m_AstarOpenList.sort([](const ENGINE_ASTAR& a, const ENGINE_ASTAR& b) {

                 return a.F < b.F;
                 });
             m_AstarCloseList.push_back(m_AstarOpenList.front());
             m_AstarOpenList.pop_front();
         }
        
         if (m_Cells[m_AstarCloseList.back().iNode_Nubmer]->CheckAstar(m_AstarCloseList.back(),
             m_AstarOpenList, m_AstarCloseList, m_Cells, DestPos, &index) )
         {
                 m_MoveToList.push_back(m_AstarCloseList.back());
                 m_AstarCloseList.pop_back();
                 while (true)
                 {
                     for (auto iter = m_AstarCloseList.begin(); iter != m_AstarCloseList.end();)
                     {

                         if (m_MoveToList.front().iParent_node == iter->iNode_Nubmer)
                         {
                             m_MoveToList.push_front(*iter);
                             iter = m_AstarCloseList.erase(iter);
                             continue;
                         }
                         ++iter;
                     }
                     if (m_MoveToList.front().iParent_node == -1)
                     {
                         m_MoveToList.pop_front();
                         m_iDestIndex = index;
                         bFinished = true;
                         return true;
                     }
                 }
         }

   

     }
     //해냈다 지렸고
     //열린 목록, 닫힌 목록
     //열린 목록은 최단거리 갱신을 위해 계속 계산 되는곳
     // F G H 값이 필요 
     // H.는 휴리틱스로 해당 목적지에 도달하는데 소요될거라고 추정되는 시간
     // G는 시작노드
     // f는 g h 더한값
     //닫힌 목록은 담는거


    return false;
}
_vector CNavigation::MoveToAstar(_fvector vPos, const _float& fSpeed, const _float& fTimeDelta, _float3* vLook)
{
    if (m_MoveToList.empty())
    {
        return vPos;
    }
    
    _vector vSrcPos{}, vDir{}, FinalPos{};
    if (!m_MoveToList.empty())
    {
        vSrcPos = XMLoadFloat3(&m_MoveToList.front().Pos);
        vDir = XMVector3Normalize(vSrcPos - XMVectorSetY(vPos,0.f));

        XMStoreFloat3(vLook, vSrcPos);
        FinalPos = vPos + vDir * fSpeed * fTimeDelta;
       //XMVectorGetX(XMVector3Length(XMVectorSetY(FinalPos, 0.f) - XMVectorSetY(vSrcPos, 0.f))) < 5.f ||
        if (XMVectorGetX(XMVector3Length(XMVectorSetY(FinalPos, 0.f) - XMVectorSetY(vSrcPos, 0.f))) < 5.f || m_iCurretnCellindex == m_MoveToList.front().iNode_Nubmer)
        {
            m_MoveToList.pop_front();

            if (!m_MoveToList.empty())
                vSrcPos = XMLoadFloat3(&m_MoveToList.front().Pos);
            else
                return vPos;
            
            vDir = XMVector3Normalize(vSrcPos- vPos);

            FinalPos = vPos + vDir * fSpeed * fTimeDelta;

        }

    }
        
    
        
    _float3 fDir{};
     if (true == InMove(FinalPos,&fDir))
         return XMVectorSetW(FinalPos,1.f);

     _vector vSlidDir = XMVector3Normalize(XMLoadFloat3(&fDir));
     _float HalfSpeed = fSpeed * 0.5f;
     _vector Move = vDir * HalfSpeed * fTimeDelta;

     //R = P + n(-P * n)    
     _vector FinalLook = XMVector3Normalize(Move + vSlidDir * (XMVectorGetX(XMVector3Dot(-Move, vSlidDir))));

     _vector vSlide = vPos + FinalLook * HalfSpeed * fTimeDelta;
     //원래 가려했던 힘 P
     if (true == InMove(vSlide))
        return  XMVectorSetW(vSlide, 1.f);

     
     _vector vRight = XMVector3Cross(XMVectorSet(0,1,0,0), XMVector3Normalize(vSlide));
     _vector vRightMove = vPos + vRight * HalfSpeed * fTimeDelta;
     _vector vRightFinal = XMVector3Normalize(vRightMove + vRight * (XMVectorGetX(XMVector3Dot(-vRightMove, vRight))));
     
     _vector vRightSlide = vPos + vRightFinal * HalfSpeed * fTimeDelta;

     if (true == InMove(vRightSlide))
         return  XMVectorSetW(vRightSlide, 1.f);

    return XMVectorSetW(vPos,1.f);
}
shared_ptr<CGameObject> CNavigation::Find_Object(const uint32_t endLayerIndex, const _wstring& LayerName, const _char* tagName)
{
    CLayer* pLayer = nullptr;
   
    pLayer = CGameInstance::Get().Find_Layer(endLayerIndex, LayerName);
    if (NULL_TRUE(pLayer))
        return nullptr;
  
    auto pObj = pLayer->Find_Object(tagName);
    if (NULL_FALSE(pObj))
        return pObj;

    return nullptr;
}
_bool CNavigation::Check_NeraPos(_float3* fPos)
{
    if (m_Cells.empty())
    {
        return false;
    }
    _float firstNearDistance{ FLT_MAX };
    EPOINT  FirstNear = {};
    int32_t iFirstCellCnt{ -1 };

    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        for (int32_t j = 0; j < 3; ++j)
        {
            _float Length = XMVectorGetX(XMVector3Length(m_Cells[i]->Get_CellPos(j) - XMLoadFloat3(fPos)));

            if (firstNearDistance > Length)
            {
                firstNearDistance = Length;
                FirstNear = static_cast<EPOINT>(j);
                iFirstCellCnt = i;
            }

        }
    }

    XMStoreFloat3(fPos ,m_Cells[iFirstCellCnt]->Get_CellPos(ETOUI(FirstNear)));
    return true;
}
void CNavigation::Add_NaviMeshInfo(_float3* fPos, CELL_EVENT eEvent)
{

    _float3 Pos[3] = {};

    memcpy(&Pos, fPos, sizeof _float3 * ETOUI(EPOINT::END));

   auto Cell = CCell::Create(m_pDevice, m_pContext,{} ,eEvent,m_Cells.size()  ,Pos);
   
   if (NULL_TRUE(Cell))
        return;
   
     m_Cells.push_back(Cell);

}
HRESULT CNavigation::Save_Navi(const _wstring& FilePath, const _char* pName)
{
   // Ready_Neightbors();

    json j;
    for (auto& iter : m_Cells)
    {
        if (iter->Save_Data() == nullptr)
            continue;

        j[pName].push_back(iter->Save_Data());
    }

    ofstream file(FilePath);
    file << j.dump(4);

    file.close();


    return S_OK;
}

HRESULT CNavigation::Load_Navi(const _wstring& FilePath, const _char* pName)
{
    m_Cells.clear();
    json j;
    ifstream file(FilePath);
    if (!file.is_open())
    {
        MSG_BOX("로드할 파일이 없음");
        return E_FAIL;
    }
    j = json::parse(file);
    int32_t index = 0;
    for (auto& iter : j[pName])
    {
        _float3 fPos[3] = {};
        NAVI eNavi{};
        CELL_EVENT eEvent{};
        for (int32_t i = 0; i < 3; ++i)
        {
            fPos[i].x = iter["Pos"][i][0];
            fPos[i].y = iter["Pos"][i][1];
            fPos[i].z = iter["Pos"][i][2];

      

        }
       //eNavi.vCenter = { iter["Center"][0],iter["Center"][1],iter["Center"][2] };
       //eNavi.m_vPlane = { iter["Plane"][0],iter["Plane"][1],iter["Plane"][2],iter["Plane"][3]};
        eNavi.iIndex = iter["MyIndex"];
        int32_t iEvent = iter["Event"];

          eEvent = static_cast<CELL_EVENT>(iEvent);
          auto Cell = CCell::Create(m_pDevice, m_pContext, {}, eEvent, m_Cells.size(), fPos);
        m_Cells.push_back(Cell);
    }
    file.close();


    for (auto Cell = m_Cells.begin(); Cell != m_Cells.end();)
    {
        
        _vector Near = { 0.001f, 0.001f, 0.001f, 0 };

        if(XMVector3NearEqual(XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[0]), XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[1]), Near) ||
            XMVector3NearEqual(XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[1]), XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[2]), Near)||
            XMVector3NearEqual(XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[0]), XMLoadFloat3(&(*Cell)->Get_NaviInfo().vPoints[2]), Near))
        {
            Cell = m_Cells.erase(Cell);
            continue;
        }

        ++Cell;
    }
    Ready_Neightbors();
    return S_OK;
}
void CNavigation::Undo_Cell()
{
    if (!m_Cells.empty())
        m_Cells.pop_back();
}
void CNavigation::Reset_Astar()
{
   	m_iDestIndex =  -1;

   	m_AstarOpenList.clear();
   	m_AstarCloseList.clear();
   	m_MoveToList.clear();
   
}
_vector CNavigation::Get_CellEventPos(CELL_EVENT eType)
{
    for (auto& iter : m_Cells)
    {
        if (iter->Event_Check(eType))
        {
            return XMLoadFloat3(&iter->Get_NaviInfo().vCenter);
        }
    }
    return XMVectorSet(0,0,0,1);
}
void CNavigation::ReSearchCell(_fvector vPos)
{
    int32_t iIndex = -1;
    for (auto& iter : m_Cells)
    {
        iIndex = iter->ReSearchCell(vPos);
        if (iIndex != -1)
        {
            m_iCurretnCellindex = iIndex;
            return;
        }
    }
}
void CNavigation::Dead_Check()
{

    for (auto iter = m_Cells.begin(); iter != m_Cells.end();)
    {
        if ((*iter)->Get_NaviInfo().bDead == true)
        {
            iter = m_Cells.erase(iter);
            continue;
        }
        ++iter;
    }
}
void CNavigation::Event_Check(CELL_EVENT eCellEvent)
{
    if (eCellEvent == CELL_EVENT::BOSSTP)
    {
        EVENT eEvent;
        eEvent.eEvent = WORLD_EVENT::BOSS_TP;
        eEvent.iIndex = m_iCurretnCellindex;
        eEvent.fPos = m_Cells[m_iCurretnCellindex]->Get_NaviInfo().vCenter;
        CGameInstance::Get().Notify(WORLD_EVENT::BOSS_TP,eEvent);
    }
    else if (eCellEvent == CELL_EVENT::BOSS_EVENT1)
    {
        EVENT eEvent;
        eEvent.eEvent = WORLD_EVENT::BOSS_EVENT1;
        eEvent.iIndex = m_iCurretnCellindex;
        eEvent.fPos = m_Cells[m_iCurretnCellindex]->Get_NaviInfo().vCenter;
        CGameInstance::Get().Notify(WORLD_EVENT::BOSS_EVENT1, eEvent);

     }else if(eCellEvent == CELL_EVENT::ARROW)
     {
         EVENT eEvent;
         eEvent.eEvent = WORLD_EVENT::BOSS_EVENT2;
         for (auto& iter : m_Cells)
         {
             if (iter->Event_Check(CELL_EVENT::BOSS_EVENT2))
             {
                 eEvent.iIndex = iter->Get_NaviInfo().iIndex;
                 eEvent.fPos = iter->Get_NaviInfo().vCenter;
                 break;
             }
         }
         CGameInstance::Get().Notify(WORLD_EVENT::BOSS_EVENT2, eEvent);

     }
    m_eEvent = eCellEvent;

}
_vector CNavigation::Find_CellPos(int32_t index)
{
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        if (m_Cells[i]->Get_NaviInfo().iIndex == index)
            return XMLoadFloat3(&m_Cells[i]->Get_NaviInfo().vCenter);
    }
    return XMVectorSet(0, 0, 0, 1);
}


const _vector CNavigation::Get_CurrentCell_Info(int32_t* iDestIndex)
{
    *iDestIndex = m_iCurretnCellindex;
    if (m_Cells.empty())
    {
        *iDestIndex = -1;
        return XMVectorSet(0,0,0,0);
    }
    return XMLoadFloat3(&m_Cells[m_iCurretnCellindex]->Get_NaviInfo().vCenter);
}
#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    _float4x4       WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
   
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", CGameInstance::Get().Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", CGameInstance::Get().Get_Transform(D3DTS::PROJ))))
        return E_FAIL;
   
    m_pShader->Begin(0);
   
    for (auto& pCell : m_Cells)
    {
        if (nullptr != pCell)
            pCell->Render(m_pShader.get());
    }
   
   _float4 fColor = { 1,1,1,1 };
   m_pShader->Bind_RawValue("g_Color", &fColor, sizeof _float4);
   
   m_pShader->Begin(0);

    return S_OK;
}
shared_ptr<class CCell> CNavigation::Select_TriAngle(_fvector vOrigin, _fvector vDir)
{

    _float fDist{};
    for (auto& iter : m_Cells)
    {

        if (TriangleTests::Intersects(vOrigin, vDir, iter->Get_CellPos(ETOUI(EPOINT::A)),
            iter->Get_CellPos(ETOUI(EPOINT::B)), iter->Get_CellPos(ETOUI(EPOINT::C)), fDist))
        {
            iter->Set_Choice(true);
            return iter;

        }

    }


    return nullptr;
}
#endif
unique_ptr<CNavigation>		CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _wstring& FilePath, const _char* pName)
{
    auto pInstance = unique_ptr<CNavigation>(new CNavigation(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(FilePath,pName)))
    {
        MSG_BOX("Create Failed CNavigation");
        return nullptr;
    }
    return pInstance;
}

shared_ptr<CPrototype> CNavigation::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<CNavigation>(new CNavigation(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        return nullptr;
    }

    return pInstance;
}
