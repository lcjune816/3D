#include "BatteryCase.h"
#include "GameObject.h"
CBatteryCase::CBatteryCase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBatteryCase::CBatteryCase(const CBatteryCase& Prototype) : CTrigger(Prototype)
{
}

CBatteryCase::~CBatteryCase()
{
}

HRESULT CBatteryCase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBatteryCase::Initialize(void* pArg)
{

	m_eEventTrigger = TRIGGER_EVENT::BATTERYCASE;
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CBatteryCase::Interaction( _float fTimeDelta,  _bool bOtherTrigger)
{
	if (m_bTriggerOn) return E_FAIL;

	return S_OK;
}
HRESULT CBatteryCase::Late_Interaction( _float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
HRESULT CBatteryCase::Action_Trigger(weak_ptr<class CTransform> pTransform)
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return E_FAIL;
	auto SrcTransform = pObj->Get_Transform().lock();
	auto DstTransform = pTransform.lock();
	if (NULL_TRUE(SrcTransform) || NULL_TRUE(DstTransform))
		return E_FAIL;

	if(CGameInstance::Get().Only_AABB_Collision(SrcTransform, DstTransform))
	{//충돌하면 배터리를 집어 넣으라
		_float3 fPos = {};
		XMStoreFloat3(&fPos,SrcTransform->Get_State(STATE::POS));
		fPos.y += 6.f;
		DstTransform->Set_State(STATE::POS,XMLoadFloat3(&fPos));
		DstTransform->Apply_Rotation(_vector{ 1,0,0,0 }, -90.f);
		m_bTriggerOn = true;
		
		return S_OK;
	}

	return E_FAIL;
}

unique_ptr<CBatteryCase>CBatteryCase::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CBatteryCase>(new CBatteryCase(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBatteryCase");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBatteryCase::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBatteryCase>(new CBatteryCase(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBatteryCase Clone");
		return nullptr;
	}

	return pInstance;

}