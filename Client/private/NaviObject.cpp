#include "NaviObject.h"
#include "GameInstance.h"
CNaviObject::CNaviObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CNaviObject::CNaviObject(const CNaviObject& Prototye) :CGameObject(Prototye)
{
}
CNaviObject::~CNaviObject()
{
}

HRESULT CNaviObject::Initialize_Prototype()
{

	return S_OK;
}
HRESULT CNaviObject::Initialize(void* pArg)
{
 	__super::Initialize(pArg);
	Create_Component(pArg);
	return S_OK;
}
void CNaviObject::Priority_Update(_float fTimeDelta)
{
}
void CNaviObject::Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CNaviObject));

}
void CNaviObject::Late_Update(_float fTimeDelta)
{

}
HRESULT CNaviObject::Render()
{
	
	return S_OK;
}


json CNaviObject::Save_Data()
{

	return nullptr;
}
HRESULT CNaviObject::Ready_Component()
{

	return S_OK;
}
HRESULT CNaviObject::Create_Component(void* pArg)
{
	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Box"), TEXT("Com_BoxShader"), m_pShaderCom)))
		return E_FAIL;



	return S_OK;
}
void CNaviObject::Load_Data(void* pDesc, const json& j)
{

}
unique_ptr<CNaviObject> CNaviObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CNaviObject>(new CNaviObject(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed TriggerObject");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CNaviObject::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CNaviObject(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed TriggerObject_Clone ");
		return pInstance;
	}
	return pInstance;
}
