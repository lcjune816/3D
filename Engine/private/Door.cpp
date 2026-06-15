#include "Door.h"
#include "GameInstance.h"
CDoor::CDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& Prototype) : CTrigger(Prototype)
{
}

CDoor::~CDoor()
{
}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);

	if (pDesc->eWroldEvent == WORLD_EVENT::DOOR)
	{
 		CGameInstance::Get().Add_Observers(pDesc->eWroldEvent, SHARED_THIS(CDoor));

	}
	else if (pDesc->eWroldEvent == WORLD_EVENT::BOSS_EVENT1)
	{
		m_bOtherTrigger = true;
		CGameInstance::Get().Add_Observers(pDesc->eWroldEvent, SHARED_THIS(CDoor));
	}

	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::DOOR;
		

	m_fEndAngle = 90.f;
	m_fMaxFrameTime = 1.f;
	m_fRotation = { 0,1,0,0 };
	Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE)| ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);
	return S_OK;
}

HRESULT CDoor::Interaction( _float fTimeDelta,  _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		if (Start_Rotation(fTimeDelta))
		{
			m_eState = TRIGGER_STATE::IDLE;
			Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
			Set_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER), FLAGVALUE::ENABLE);
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
	case TRIGGER_STATE::WORLD:
		if(Start_Rotation(fTimeDelta))
			m_eState = TRIGGER_STATE::IDLE;
		else
			Action_Trigger();
		break;
	}
	return S_OK;
}
HRESULT CDoor::Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger)
{
	return S_OK;
}
void CDoor::Set_Trigger()
{

	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		IS_PLAYSOUND(DOOR_SOUND, CHANNELID::SOUND_OBJECT, 0.3f);
		m_eState = TRIGGER_STATE::ACTION;
		return;
	}
	else if (Check_Flag(ETOUI(TRIGGER_FLAG::OTHERTRIGGER)))
	{

		IS_PLAYSOUND(DOOR_SOUND, CHANNELID::SOUND_OBJECT, 0.3f);
		m_eState = TRIGGER_STATE::RETURN;
		return;
	}
}
void CDoor::OnNotify(const EVENT& event)
{
	if (!Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		return;
	
	if (event.eEvent == WORLD_EVENT::DOOR)
	{
		m_eState = TRIGGER_STATE::WORLD;
		m_fEndAngle = 140.f;
		m_fMaxFrameTime = 1.f;
		m_fRotation = { 0,1,0,0 };
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	}
	else if(event.eEvent == WORLD_EVENT::BOSS_EVENT1)
	{
		CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_EVENT2, SHARED_THIS(CDoor));
	}
	else if (event.eEvent == WORLD_EVENT::BOSS_EVENT2)
	{
		m_eState = TRIGGER_STATE::WORLD;
		m_fEndAngle = 140.f;
		m_fMaxFrameTime = 1.f;
		m_fRotation = { 0,1,0,0 };
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	}
}
void CDoor::Action_Trigger()
{
	auto pObj = m_pParent.lock();
	if (NULL_TRUE(pObj))
		return;

	pObj->Get_Transform().lock()->Rotation_Origin(XMLoadFloat4(&m_fRotation), m_fAngle);
}

unique_ptr<CDoor>CDoor::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CDoor>(new CDoor(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CDoor");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CDoor::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CDoor>(new CDoor(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CDoor Clone");
		return nullptr;
	}

	return pInstance;

}