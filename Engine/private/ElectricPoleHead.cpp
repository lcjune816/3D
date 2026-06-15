#include "ElectricPoleHead.h"
#include "GameObject.h"
#include "GameInstance.h"
CElectricPoleHead::CElectricPoleHead(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CElectricPoleHead::CElectricPoleHead(const CElectricPoleHead& Prototype) : CTrigger(Prototype)
{
}

CElectricPoleHead::~CElectricPoleHead()
{
}

HRESULT CElectricPoleHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectricPoleHead::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	__super::Initialize(pArg);

	m_eEventTrigger = TRIGGER_EVENT::POLEHEAD;
	m_fRotationArrow = 10.f;
	m_bOtherTrigger = true;
	return S_OK;
}
HRESULT CElectricPoleHead::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;

}
HRESULT CElectricPoleHead::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	Action_Trigger();
	return S_OK;
}
HRESULT CElectricPoleHead::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}
void CElectricPoleHead::Set_Trigger()
{
	Set_Flag(ETOUI(TRIGGER_FLAG::SHADER) | ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
}
_bool CElectricPoleHead::offsetMatrix(_float4x4* pMatrix)
{
	return true;
}
void CElectricPoleHead::Action_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iLevel,m_iTargetNumber).lock();
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		if (NULL_FALSE(TriggerCheck))
		{
		
			if (TriggerCheck->Check_Trigger_Event(TRIGGER_EVENT::PANNEL))
				TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
			else TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);
		}
			
		m_BindValue.fColor = { 0,0,1,1 };
	}
	else
	{
		if (NULL_FALSE(TriggerCheck))
			TriggerCheck->Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER) | ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
		m_BindValue.fColor = { 1,1,1,1 };
	}


}

unique_ptr<CElectricPoleHead>  CElectricPoleHead::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<CElectricPoleHead>(new CElectricPoleHead(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CElectricPoleHead");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CElectricPoleHead::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CElectricPoleHead>(new CElectricPoleHead(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CElectricPoleHead Clone");
		return nullptr;
	}

	return pInstance;

}