#include "ElectricPole.h"
#include "GameObject.h"
#include "GameInstance.h"
CElectricPole::CElectricPole(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CElectricPole::CElectricPole(const CElectricPole& Prototype) : CTrigger(Prototype)
{
}

CElectricPole::~CElectricPole()
{
}

HRESULT CElectricPole::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectricPole::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);

	m_eEventTrigger = TRIGGER_EVENT::ELECTRICPOLE;
	m_fRotationArrow = 10.f;
	m_bOtherTrigger = true;
	return S_OK;
}
HRESULT CElectricPole::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	m_bOtherTrigger = false;
	return S_OK;

}
HRESULT CElectricPole::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
HRESULT CElectricPole::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지

	_bool Shader = false;
	if (!m_bOtherTrigger)
		m_bTriggerOn = false;

	if (m_bTriggerOn)
		Shader = true;

	Action_Trigger(Shader);

	return S_OK;
}
_bool CElectricPole::offsetMatrix(_float4x4* pMatrix)
{
	return true;
}
void CElectricPole::Action_Trigger(_bool bShader)
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (bShader)
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Trigger();
		m_BindValue.fColor = { 0,1,0,1 };
		Set_Flag(TRIGGER_FLAG::SHADER, FLAGVALUE::ENABLE);
	}
	else
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Trigger(false);
		m_BindValue.fColor = { 1,1,1,1 };
		Set_Flag(TRIGGER_FLAG::SHADER, FLAGVALUE::DISABLE);
		Disconnect_Transform();
		m_bTriggerOn = false;
	}

}

unique_ptr<CElectricPole>  CElectricPole::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CElectricPole>(new CElectricPole(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CElectricPole");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CElectricPole::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CElectricPole>(new CElectricPole(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CElectricPole Clone");
		return nullptr;
	}

	return pInstance;

}