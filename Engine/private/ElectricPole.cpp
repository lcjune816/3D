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
	Set_Flag(ETOUI(TRIGGER_FLAG::SHADER) | ETOUI(TRIGGER_FLAG::FTRIGGER),FLAGVALUE::DISABLE);
	return S_OK;

}
HRESULT CElectricPole::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	Action_Trigger();
	return S_OK;
}
HRESULT CElectricPole::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{//손에 전기 뭍은상태로 닿으면 문 열리게 바꿔야지


	return S_OK;
}
void CElectricPole::Set_Trigger()
{
	Set_Flag(ETOUI(TRIGGER_FLAG::SHADER) | ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
}
_bool CElectricPole::offsetMatrix(_float4x4* pMatrix)
{
	return true;
}
void CElectricPole::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Trigger();
	}
	else
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::SHADER) | ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
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