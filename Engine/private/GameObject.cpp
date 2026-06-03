#include "GameObject.h"

CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice{pDevice},m_pContext{pContext}
{

}
CGameObject::CGameObject(const CGameObject& Prototype) : m_pDevice(Prototype.m_pDevice),m_pContext{Prototype.m_pContext}
{

}
CGameObject::~CGameObject()
{

}

HRESULT CGameObject::Initialize(void* pArg)
{
	

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (FAILED(m_pTransform->Initialize_Prototype()))
		return E_FAIL;


	if (nullptr == pArg)
		return S_OK;

	auto Desc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	m_eMeshType = Desc->eType;
	if (!Desc->bFrontCamera)
		Desc->bWorldCheck = false;

	m_pTransform->Initialize(pArg);

	return S_OK;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{

}

void CGameObject::Update(_float fTimeDelta)
{

}

void CGameObject::Late_Update(_float fTimeDelta)
{

}

shared_ptr<class CComponent> CGameObject::Find_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}
void CGameObject::Load_Data(void* pDesc, const json& j)
{
	return ;
}
json CGameObject::Save_Data()
{
	return "";
}

HRESULT CGameObject::Render()
{

	return S_OK;
}

void CGameObject::ReBindsComponnet(uint32_t iLevelIndex)
{
}


_bool CGameObject::Check_Name(const _char* pName)
{
	if (!strcmp(pName, m_pTagName))
	return true;   

	return false; 
}