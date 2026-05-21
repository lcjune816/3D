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
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return false;

    pNavi->Check_NeraPos(fPos);

}
void CNavi_Manager::Render()
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return ;
    pNavi->Render();
}
void CNavi_Manager::Add_NaviMeshInfo( _float3* fPos, CELL_EVENT eEvent)
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return;

    pNavi->Add_NaviMeshInfo(fPos, eEvent);
       
}
_bool	CNavi_Manager::Check_First()
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return false;

   return pNavi->Check_First();
}
HRESULT CNavi_Manager::Save_Navi(const _wstring& FilePath, const _char* pName)
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return E_FAIL;

     pNavi->Save_Navi(FilePath, pName);

     return S_OK;
}

HRESULT CNavi_Manager::Load_Navi(const _wstring& FilePath, const _char* pName)
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return E_FAIL;

    pNavi->Load_Navi(FilePath, pName);

    return S_OK;
}
void CNavi_Manager::Undo_Cell()
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return;

    pNavi->Undo_Cell();
}
#ifdef _DEBUG

void CNavi_Manager::Connect_Navigaion(shared_ptr<class CNavigation> pNavigation)
{
    if (NULL_FALSE(pNavigation))
        m_pNavigations = pNavigation;
}
shared_ptr<class CCell> CNavi_Manager::Select_TriAngle(_fvector vOrigin, _fvector vDir)
{
    auto pNavi = m_pNavigations.lock();
    if (NULL_TRUE(pNavi))
        return nullptr;

    return pNavi->Select_TriAngle(vOrigin,vDir);
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