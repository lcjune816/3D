#include "Lever.h"
#include "GameObject.h"
CLever::CLever(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CLever::CLever(const CLever& Prototype) : CTrigger(Prototype)
{
}

CLever::~CLever()
{
}

HRESULT CLever::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLever::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::LEVER;
	Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE) | ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	return S_OK;
}

HRESULT CLever::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		if (Start_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::PAUSE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER) | ETOUI(TRIGGER_FLAG::PAUSE), FLAGVALUE::ENABLE);
			TriggerToTrigger();
		}
		else
			Action_Trigger();
		break;
	case TRIGGER_STATE::RETURN:
		if (End_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::IDLE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::DISABLE);
		}
		else
			Action_Trigger();
		break;

	case TRIGGER_STATE::PAUSE:

		break;
	}
	return S_OK;
}
HRESULT CLever::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger )
{
	return S_OK;
}

void CLever::TriggerToTrigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iTargetNumber).lock();
	if (NULL_TRUE(TriggerCheck))
		return;

	TriggerCheck->TriggerToTrigger();
}

void CLever::Set_Trigger()
{
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		m_eState = TRIGGER_STATE::ACTION;
		return;
	}
	else if (Check_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER)))
	{
		Set_Flag(ETOUI(TRIGGER_FLAG::PAUSE), FLAGVALUE::DISABLE);
		m_eState = TRIGGER_STATE::RETURN;
		return;
	}
}

void CLever::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Apply_Rotation(XMLoadFloat4(&m_fRotation), m_fAngle);
}

unique_ptr<CLever>CLever::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CLever>(new CLever(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CLever");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CLever::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CLever>(new CLever(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CLever Clone");
		return nullptr;
	}

	return pInstance;

}