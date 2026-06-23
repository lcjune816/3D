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

HRESULT CCell::Initialize(NAVI Navi, CELL_EVENT eEvent, int32_t iIndex, _float3* pPoints)
{
    pPoints == nullptr ? Ready_Load(Navi, eEvent, iIndex) : Ready_New(iIndex, pPoints);
    m_eEvent = eEvent;
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
HRESULT CCell::Ready_New(int32_t iIndex, _float3* pPoints)
{
    m_NaviInfo.iIndex = iIndex;
    memcpy(m_NaviInfo.vPoints, pPoints, sizeof(_float3) * ETOUI(EPOINT::END));

    for (int32_t i = 0; i < 3; ++i)
    {
        if(m_NaviInfo.vPoints[i].y ==0)
            m_NaviInfo.vPoints[i].y +=0.8f;
    }
    _float fDaot = ((m_NaviInfo.vPoints[1].x - m_NaviInfo.vPoints[0].x) * (m_NaviInfo.vPoints[2].z - m_NaviInfo.vPoints[0].z)) - ((m_NaviInfo.vPoints[1].z - m_NaviInfo.vPoints[0].z) * (m_NaviInfo.vPoints[2].x - m_NaviInfo.vPoints[0].x)); 
    //XMVectorGetX(XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&m_NaviInfo.vPoints[1]) - XMLoadFloat3(&m_NaviInfo.vPoints[0])), XMVector3Normalize(XMLoadFloat3(&m_NaviInfo.vPoints[2]) - XMLoadFloat3(&m_NaviInfo.vPoints[0]))));
    //((m_NaviInfo.vPoints[1].x - m_NaviInfo.vPoints[0].x) * (m_NaviInfo.vPoints[2].z - m_NaviInfo.vPoints[0].z)) - ((m_NaviInfo.vPoints[1].z - m_NaviInfo.vPoints[0].z) * (m_NaviInfo.vPoints[2].x - m_NaviInfo.vPoints[0].x));
    if (fDaot > 0)
        swap(m_NaviInfo.vPoints[1], m_NaviInfo.vPoints[2]);

    _vector vLine{};
    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]);
    m_NaviInfo.vNormals[ETOUI(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0,  XMVectorGetX(vLine));
    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]);
    m_NaviInfo.vNormals[ETOUI(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0, XMVectorGetX(vLine));

    vLine = XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]) - XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]);
    m_NaviInfo.vNormals[ETOUI(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0, XMVectorGetX(vLine));

 
    for (size_t i = 0; i < ETOUI(LINE::END); ++i)
    {
        XMStoreFloat3(&m_NaviInfo.vNormals[i],
            XMVector3Normalize(XMLoadFloat3(&m_NaviInfo.vNormals[i])));

    }

     
    //평면 방정식 ax by cz d 에서 a b c 를 구하는 함수
    XMStoreFloat4(&m_NaviInfo.m_vPlane, XMPlaneFromPoints(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]),
        XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]),
        XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)])));
    
    XMStoreFloat3(&m_NaviInfo.vCenter, (XMLoadFloat3(&m_NaviInfo.vPoints[0]) + XMLoadFloat3(&m_NaviInfo.vPoints[1])
        + XMLoadFloat3(&m_NaviInfo.vPoints[2])) / 3.f);

    for (int32_t i = 0; i < 3; ++i)
    {
        _vector vLook = XMVector3Normalize(XMLoadFloat3(&m_NaviInfo.vCenter) - XMLoadFloat3(&m_NaviInfo.vPoints[i]));

        if(0 < XMVectorGetX(XMVector3Dot(vLook, XMLoadFloat3(&m_NaviInfo.vNormals[i]))))
        {
            XMStoreFloat3(&m_NaviInfo.vNormals[i], XMLoadFloat3(&m_NaviInfo.vNormals[i]) * -1.f);
            break;
        }
    }
    
    return S_OK;
}
HRESULT CCell::Ready_Load(NAVI Navi, CELL_EVENT eEvent,int32_t iIndex)
{
    m_NaviInfo.iIndex = iIndex;
    m_NaviInfo.m_vPlane = Navi.m_vPlane;
    //m_NaviInfo.vCenter = Navi.vCenter;
    
    m_NaviInfo.iNeighborIndices[0] = -1;
    m_NaviInfo.iNeighborIndices[1] = -1;
    m_NaviInfo.iNeighborIndices[2] = -1;
   
    //memcpy(&m_NaviInfo.iNeighborIndices[i], &Navi.iNeighborIndices[i], sizeof m_NaviInfo.iNeighborIndices);
    //memcpy(&m_NaviInfo.vNormals[i],&Navi.vNormals[i],sizeof _float3);
    memcpy(&m_NaviInfo.vPoints, &Navi.vPoints, sizeof _float3 * ETOUI(EPOINT::END));

     Ready_New(iIndex, &m_NaviInfo.vPoints[0]);
    m_eEvent = eEvent;
    return S_OK;
}
_bool CCell::CheckAstar(ENGINE_ASTAR& parentsNode, list<ENGINE_ASTAR>& OpenList, list<ENGINE_ASTAR>& CloseList , vector<shared_ptr<CCell>>& CellList,  _fvector vFinalPos, int32_t* iSrcIndex)
{
    int a = m_NaviInfo.iIndex;
    for (int32_t i = 0; i < 3; ++i)
    {
        if (-1 != m_NaviInfo.iNeighborIndices[i])
        {
            ENGINE_ASTAR Astar{};
            _bool bCloseCheck{ false };
            if (CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().iIndex == *iSrcIndex)
            {
                Astar.iParent_node = parentsNode.iNode_Nubmer;
                Astar.iNode_Nubmer = m_NaviInfo.iNeighborIndices[i];
                XMStoreFloat3(&Astar.Pos, XMLoadFloat3(&CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vCenter));
                CloseList.push_back(Astar);
                return true;
            }

            //닫힌 목록에 없는거만 검사
            for (auto& iter : CloseList)
            {
                if (iter.iNode_Nubmer == m_NaviInfo.iNeighborIndices[i])
                {
                    bCloseCheck= true;
                    break;
                }
            }

            if (bCloseCheck)
                continue;

            //내 이웃 노드의 위치랑 마지막 노드 위치랑 h값 구하고
            Astar.H = XMVectorGetX(XMVector3Length(vFinalPos - XMLoadFloat3(&CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vCenter)));
            //내 위치 기준으로 구하고
            Astar.G = XMVectorGetX(XMVector3Length(XMLoadFloat3(&CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vCenter) - XMLoadFloat3(&m_NaviInfo.vCenter)));
            Astar.F = Astar.H + Astar.G;
            Astar.Pos = CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vCenter;
            Astar.iNode_Nubmer = m_NaviInfo.iNeighborIndices[i];
            Astar.iParent_node = parentsNode.iNode_Nubmer;
         
            //uint32_t iCnt = 0;
            //for (int32_t j = 0; j < 3; ++j)
            //{
            //    for (int32_t k = 0; k < 3; ++k)
            //    {
            //        if (Compare_Points(XMLoadFloat3(&m_NaviInfo.vPoints[j]), XMLoadFloat3(&CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vPoints[k])))
            //        {
            //            Astar.Arrow[iCnt] = CellList[m_NaviInfo.iNeighborIndices[i]]->Get_NaviInfo().vPoints[k];
            //            ++iCnt;
            //        }
            //        if (iCnt >= 2)
            //            break;
            //    }
            //
            //}

            if (!OpenList.empty())
            {//열린 노드에서 중복된거 있는지 확인
                _bool OpenCheck{ false };
                for (auto iter = OpenList.begin(); iter != OpenList.end();)
                {
                    if (iter->iNode_Nubmer == m_NaviInfo.iNeighborIndices[i])
                    {
                        if (iter->G > Astar.G )//|| fabsf(iter->G - Astar.G) < 0.0001f )
                        {
                            OpenCheck = false;
                            break;
                        }
                        else if(iter->G < Astar.G)
                        {
                            OpenCheck = true;
                            iter = OpenList.erase(iter);                 
                            break;
                        }

                    }else
                        OpenCheck = true;

                    ++iter;
                }

                if (OpenCheck)
                    OpenList.push_back(Astar);
            }else
                OpenList.push_back(Astar);
            //열린 목록 검사
        }
    }
    return false;
}
_bool CCell::IsIn(_fvector vResultPos, int32_t* pNeighborIndex,_float3* vNormal)
{

    for (size_t i = 0; i < ETOUI(LINE::END); ++i)
    {
        _vector     vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_NaviInfo.vPoints[i]));
        _float fDot = XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_NaviInfo.vNormals[i])));
        if (0.001f <  fDot)
        {//만약 벗어낫을때 내가 가지고있는 이웃 인덱스를 전달
            if(NULL_FALSE(vNormal))
                *vNormal = m_NaviInfo.vNormals[i];
            *pNeighborIndex = m_NaviInfo.iNeighborIndices[i];
            return false;
        }
    }


    return true;
}
_bool   CCell::Event_Check(CELL_EVENT eEvent)
{
    if (m_eEvent == eEvent)
        return true;

    return false;
}
_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
    //내 A정점이랑 Src 위치랑 같냐
    _vector Equal = { 0.001f,0.001f ,0.001f ,0 };
    if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]), vSourPoint,Equal))
    {
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]), vDestPoint, Equal))
            return true;
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]), vDestPoint, Equal))
            return true;

    }

    if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]), vSourPoint, Equal))
    {
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]), vDestPoint, Equal))
            return true;
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]), vDestPoint, Equal))
            return true;

    }

    if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::C)]), vSourPoint, Equal))
    {
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::A)]), vDestPoint, Equal))
            return true;
        if (true == XMVector3NearEqual(XMLoadFloat3(&m_NaviInfo.vPoints[ETOUI(EPOINT::B)]), vDestPoint,Equal))
            return true;

    }

    return false;
}
_float CCell::Compute_Height(_fvector vPos)
{
    //ax + by + cz +d  =0
    return ( - m_NaviInfo.m_vPlane.x * XMVectorGetX(vPos)
          - m_NaviInfo.m_vPlane.z * XMVectorGetZ(vPos) -m_NaviInfo.m_vPlane.w)
        /m_NaviInfo.m_vPlane.y;
}
json CCell::Save_Data()
{
    nlohmann::json j;
    for (int32_t i = 0; i < 3; ++i)
    {
        j["Pos"][i] = { m_NaviInfo.vPoints[i].x,m_NaviInfo.vPoints[i].y,m_NaviInfo.vPoints[i].z };
        j["Normal"][i] = { m_NaviInfo.vNormals[i].x,m_NaviInfo.vNormals[i].y,m_NaviInfo.vNormals[i].z };
        
    }
    j["Center"] = { m_NaviInfo.vCenter.x,m_NaviInfo.vCenter.y,m_NaviInfo.vCenter.z };
    j["Neightbor"] = { m_NaviInfo.iNeighborIndices[0],m_NaviInfo.iNeighborIndices[1],m_NaviInfo.iNeighborIndices[2] };
    j["MyIndex"] =  m_NaviInfo.iIndex;
    j["Plane"] = { m_NaviInfo.m_vPlane.x,m_NaviInfo.m_vPlane.y,m_NaviInfo.m_vPlane.z ,m_NaviInfo.m_vPlane.w};
    int32_t iEvent = ETOUI(m_eEvent);
    j["Event"] = iEvent;
    return j;
}

int32_t CCell::ReSearchCell(_fvector vPos)
{
    
        int32_t iCnt = 0;
        _float fDis{};
        _vector vDir = XMVector3Normalize(XMVectorSet(0, -1, 0, 0) - vPos);
        if (TriangleTests::Intersects(vPos, vDir, XMLoadFloat3(&m_NaviInfo.vPoints[0]), XMLoadFloat3(&m_NaviInfo.vPoints[1]),
            XMLoadFloat3(&m_NaviInfo.vPoints[2]), fDis))
            return m_NaviInfo.iIndex;

    return -1;
}

HRESULT CCell::Render(CShader* pShader)
{
    
    _float4 fColor = { 1.f,1.f,1.f,1.f };

    if (m_bChoice)
        fColor = { 1,0,0,1 };
 
    pShader->Bind_RawValue("g_Color", &fColor, sizeof _float4);

    if (m_bChoice)
        pShader->Begin(1);
    else
        pShader->Begin(0);
    
        
    m_pVIBuffer->Bind_Resource();

    m_pVIBuffer->Render();
   

    return S_OK;
}
shared_ptr<CCell> CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, NAVI Navi, CELL_EVENT eEvent, int32_t iIndex, _float3* pPoints)
{
    auto		pInstance = shared_ptr<CCell>(new CCell(pDevice, pContext));

    if (FAILED(pInstance->Initialize(Navi, eEvent, iIndex, pPoints)))
    {
        MSG_BOX("Failed to Created : CCell");
        return nullptr;
    }

    return pInstance;
}