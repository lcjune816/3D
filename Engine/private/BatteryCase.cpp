#include "BatteryCase.h"
#include "GameInstance.h"
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
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CBatteryCase::Interaction(shared_ptr<CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger)
{
	if (m_bTriggerOn) return E_FAIL;

	m_pTransform = pTransform;
	return S_OK;
}

HRESULT CBatteryCase::Action_Trigger(weak_ptr<CTransform> pTransform)
{
	auto SrcTransform = m_pTransform.lock();
	auto DstTransform = pTransform.lock();
	if (NULL_TRUE(SrcTransform) || NULL_TRUE(DstTransform))
		return E_FAIL;

	if(CGameInstance::Get().Only_AABB_Collision(m_pTransform, pTransform))
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