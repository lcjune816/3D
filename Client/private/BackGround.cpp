#include "BackGround.h"
#include "GuiObject.h"
#include "NonModel.h"
#include "GameInstance.h"
CBackGround::CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext):
CGameObject(pDevice, pContext)
{

}
CBackGround::CBackGround(const CBackGround& Prototye):CGameObject(Prototye)
{
}
CBackGround::~CBackGround() {};


HRESULT CBackGround::Initialize_Prototype()
{
	
	return S_OK;
}
HRESULT CBackGround::Initialize(void* pArg)
{

	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 0.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;


	return S_OK;
}
void CBackGround::Priority_Update(_float fTimeDelta)
{
}
void CBackGround::Update(_float fTimeDelta)
{
		
}
void CBackGround::Late_Update(_float fTimeDelta)
{
	
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::PRIORITY, SHARED_THIS(CBackGround));
}
HRESULT CBackGround::Render()
{
	return S_OK;
}


unique_ptr<CBackGround> CBackGround::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CBackGround>(new CBackGround(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CBackGround::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CBackGround(*this));
	
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
