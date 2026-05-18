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
    _float SrcLen = XMVectorGetX(XMVector3Length(XMLoadFloat3(&fPos[0])));
    size_t iFirstCellCnt{};
    size_t iSecondCellCnt{};
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _float CheckNear{};
        _float ALength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::A)]) - XMLoadFloat3(&fPos[0])));
        _float BLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::B)]) - XMLoadFloat3(&fPos[0])));
        _float CLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::C)]) - XMLoadFloat3(&fPos[0])));
        
        CheckNear = min(ALength, min(BLength, CLength));
        
        if (firstNearDistance > CheckNear)
        {
            firstNearDistance = CheckNear;

            if (CheckNear == ALength)
                FirstNear = EPOINT::A;
            else if (CheckNear == BLength)
                FirstNear = EPOINT::B;
            else if (CheckNear == CLength)
                FirstNear = EPOINT::C;

            iFirstCellCnt = i;
        }

    }
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _float CheckNear{};
        _float ALength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::A)]) - XMLoadFloat3(&fPos[0])));
        _float BLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::B)]) - XMLoadFloat3(&fPos[0])));
        _float CLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Cells[i]->Get_CellPos()[ETOUI(EPOINT::C)]) - XMLoadFloat3(&fPos[0])));
        
        CheckNear = min(ALength, min(BLength, CLength));
       
        if (firstNearDistance != CheckNear)
        {
            if (CheckNear == ALength)
                SecondNear = EPOINT::A;
            else if (CheckNear == BLength)
                SecondNear = EPOINT::B;
            else if (CheckNear == CLength)
                SecondNear = EPOINT::C;

            iSecondCellCnt = i;
        }
        else
        {

            if (firstNearDistance == ALength)      
              SecondNear =  BLength == min(BLength, CLength) ? EPOINT::B : EPOINT::C;
            else if (firstNearDistance == BLength)
              SecondNear =  ALength == min(ALength, CLength) ? EPOINT::A : EPOINT::C;
            else if (firstNearDistance == CLength)
              SecondNear =  ALength == min(ALength, BLength) ? EPOINT::A : EPOINT::B;

                iSecondCellCnt = i;
        }
    }
    fPos[ETOUI(EPOINT::B)] = m_Cells[iFirstCellCnt]->Get_CellPos()[ETOUI(FirstNear)];
    fPos[ETOUI(EPOINT::C)] = m_Cells[iSecondCellCnt]->Get_CellPos()[ETOUI(SecondNear)];
    Add_NaviMeshInfo(fPos);
    return true;
}
void CNavi_Manager::Add_NaviMeshInfo( _float3* fPos)
{
    auto Cell = CCell::Create(m_pDevice, m_pContext, fPos, m_Cells.size());
       if(NULL_TRUE(Cell))
            return;

       m_Cells.push_back(Cell);

}
void CNavi_Manager::Save_Navi(json& j)
{
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
            pCell->Render();
    }

    return S_OK;
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