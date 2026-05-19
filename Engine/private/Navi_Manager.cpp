#include "Navi_Manager.h"
#include "GameInstance.h"
CNavi_Manager::CNavi_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice), m_pContext(pContext)
{
}

CNavi_Manager::~CNavi_Manager()
{
}

HRESULT CNavi_Manager::Initialize()
{
#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTX_POS::Elements, VTX_POS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;

}
_bool CNavi_Manager::Check_NeraPos(_float3* fPos)
{
    if (m_Cells.empty())
    {
        Add_NaviMeshInfo(fPos);
        return true;
    }
    _float firstNearDistance{ FLT_MAX };
    _float SecondNearDistance{ FLT_MAX };
     
    EPOINT  FirstNear = {};
    EPOINT  SecondNear = {};
    int32_t iFirstCellCnt{-1};
    int32_t iSecondCellCnt{-1};
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        for (int32_t j = 0; j < 3; ++j)
        {
            _float Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[j]) - XMLoadFloat3(&fPos[0])));

            if (Length < 0.0001f)
                continue;

            if (firstNearDistance > Length)
            {
                firstNearDistance = Length;
                FirstNear = static_cast<EPOINT>(j);
                iFirstCellCnt = i;
            }

        } 
    }
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        for (int32_t j = 0; j < 3; ++j)
        {
            if ((i == iFirstCellCnt) && (j == ETOUI(FirstNear)))
                continue;

            _float Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[j]) - XMLoadFloat3(&fPos[3])));
          
            if (Length < 0.0001f || (fabsf(Length - firstNearDistance )< 0.1f))
                continue;


            if (SecondNearDistance > Length)
            {
       
               SecondNearDistance = Length;
                SecondNear = static_cast<EPOINT>(j);
                iSecondCellCnt = i;
            }

        }
    }
    
    if (iFirstCellCnt == -1 || iSecondCellCnt == -1)
        return false;

    fPos[1] = m_Cells[iFirstCellCnt]->Get_CellPos()[ETOUI(FirstNear)];
    fPos[2] = m_Cells[iSecondCellCnt]->Get_CellPos()[ETOUI(SecondNear)];
    Add_NaviMeshInfo(fPos);
    return true;
}
void CNavi_Manager::Add_NaviMeshInfo( _float3* fPos)
{
    _float3 Points[4] = {};
    for (int32_t i = 0; i < 4; ++i)
    {              
        Points[i] = fPos[i];
    }
    _float CenterX =(Points[0].x + Points[1].x + Points[2].x + Points[3].x) * 0.25f;
    _float CenterZ =(Points[0].z + Points[1].z + Points[2].z + Points[3].z) * 0.25f;
  
    sort(begin(Points), end(Points), [CenterX, CenterZ](_float3 a, _float3 b) {
        _float at = atan2f(a.z - CenterZ, a.x - CenterX);
        _float bt = atan2f(b.z - CenterZ, b.x - CenterX);
        return at < bt;
        });
    _float3 V0[3], V1[3];
    V0[0] = Points[0];
    V0[1] = Points[1];
    V0[2] = Points[2];

    V1[0] = Points[0];
    V1[1] = Points[2];
    V1[2] = Points[3];

    auto Cell = CCell::Create(m_pDevice, m_pContext, &V0[0], m_Cells.size());
    auto Cell2 = CCell::Create(m_pDevice, m_pContext, &V1[0], m_Cells.size());

    if(NULL_TRUE(Cell) || NULL_TRUE(Cell2))
        return;

    m_Cells.push_back(Cell);
    m_Cells.push_back(Cell2);
       
}
HRESULT CNavi_Manager::Save_Navi(const _wstring& FilePath, const _char* pName)
{
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
    
     MSG_BOX("된듯?");

     return S_OK;
}

HRESULT CNavi_Manager::Load_Navi(const _wstring& FilePath, const _char* pName)
{
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
        for (int32_t i = 0; i < 3; ++i)
        {
           fPos[i].x = iter["Pos"][i][0];
           fPos[i].y = iter["Pos"][i][1];
           fPos[i].z = iter["Pos"][i][2];

        }
        auto Cell = CCell::Create(m_pDevice, m_pContext, &fPos[0], m_Cells.size());
        m_Cells.push_back(Cell);
    }

    file.close();

    MSG_BOX("로드 된듯?");
    return S_OK;
}
void CNavi_Manager::Undo_Cell()
{
    if (!m_Cells.empty())
        m_Cells.pop_back();
}
#ifdef _DEBUG
HRESULT CNavi_Manager::Render()
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

    return S_OK;
}
shared_ptr<class CCell> CNavi_Manager::Select_TriAngle(_fvector vOrigin, _fvector vDir)
{
    
    _float fDist{};
    for (auto& iter : m_Cells)
    {

        if (TriangleTests::Intersects(vOrigin, vDir, XMLoadFloat3(&iter->Get_CellPos()[0]),
            XMLoadFloat3(&iter->Get_CellPos()[1]), XMLoadFloat3(&iter->Get_CellPos()[2]), fDist))
        {
            iter->Set_Choice(true);
            return iter;

        }
            
    }


    return nullptr;
}
#endif
unique_ptr<CNavi_Manager>		CNavi_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CNavi_Manager>(new CNavi_Manager(pDevice,pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed NaviManager");
		return nullptr;
	}
	return pInstance;
}