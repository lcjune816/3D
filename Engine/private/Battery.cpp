#include "Battery.h"
#include "GameInstance.h"
#include "BatteryCase.h"
CBattery::CBattery(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CBattery::CBattery(const CBattery& Prototype) : CTrigger(Prototype)
{
}

CBattery::~CBattery()
{
}

HRESULT CBattery::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattery::Initialize(void* pArg)
{

	m_eEventTrigger = TRIGGER_EVENT::BATTERY;
	m_fRotationArrow = 0.25f;
	return S_OK;
}

HRESULT CBattery::Interaction(shared_ptr<CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger)
{
	if (!m_bTriggerOn) return E_FAIL;
	_vector vPos{};

	if (m_pDstTransform != nullptr)
	{
		vPos = m_pDstTransform->Get_State(STATE::POS);
	}
	else
	{
		vPos =  pTransform->Get_State(STATE::POS);
		_float3 Pos{};
		XMStoreFloat3(&Pos, vPos);

		_float fY = pTransform->Get_Min().y;
		//배터리는 자기 Pivot Y축 만큼 땅 위로 올리기
		m_fDropTime += 9.8f * fTimeDelta;
		if (Pos.y < -fY)
		{
			Pos.y = -fY;
			m_fDropTime = 0;
		}
		else if (Pos.y > -fY)
			Pos.y -= m_fDropTime;

		vPos = XMLoadFloat3(&Pos);
	}


	pTransform->Set_State(STATE::POS, vPos);

	auto Target = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_TRUE(Target))
		return E_FAIL;

	if (0 == static_pointer_cast<CBatteryCase>(Target)->Action_Trigger(pTransform))
	{
		m_pDstTransform = nullptr;
		m_bTriggerOn = false;
	}
		
	return S_OK;
}

void CBattery::Action_Trigger(shared_ptr<CTransform> pTransform)
{
}

unique_ptr<CBattery>CBattery::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CBattery>(new CBattery(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CBattery");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CBattery::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CBattery>(new CBattery(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CBattery Clone");
		return nullptr;
	}

	return pInstance;

}