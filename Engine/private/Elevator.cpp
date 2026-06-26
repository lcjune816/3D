#include "Elevator.h"
#include "GameInstance.h"
CElevator::CElevator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CElevator::CElevator(const CElevator& Prototype) : CTrigger(Prototype)
{
}

CElevator::~CElevator()
{
}

HRESULT CElevator::Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		Action_Trigger(fTimeDelta);
		break;
	case TRIGGER_STATE::RETURN:
		break;

	case TRIGGER_STATE::PAUSE:
		Pause_Trigger();
		break;
	case TRIGGER_STATE::END:
		break;

	}
	return S_OK;
}

HRESULT CElevator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElevator::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::ELEVATOR;
	auto pDesc = static_cast<TRIGGER_DESC*>(pArg);
	m_eState = TRIGGER_STATE::IDLE;
	Set_Flag(ETOUI(TRIGGER_FLAG::CANCLE), FLAGVALUE::ENABLE);

	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_EVENT1, SHARED_THIS(CElevator));

	return S_OK;
}

HRESULT CElevator::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	return S_OK;
}
HRESULT CElevator::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{

	return S_OK;
}

void CElevator::TriggerToTrigger()
{
	EVENT eEvent;
	eEvent.eEvent = WORLD_EVENT::BOSS_TP;
	eEvent.iIndex = m_iIndex;
	eEvent.fPos = m_vPos;
	CGameInstance::Get().Notify(WORLD_EVENT::BOSS_TP, eEvent);
	m_eState = TRIGGER_STATE::ACTION;
}

void CElevator::Set_Trigger()
{
}

void CElevator::OnNotify(const EVENT& eEvent)
{
	if (eEvent.eEvent == WORLD_EVENT::BOSS_EVENT1)
	{
		m_iIndex = eEvent.iIndex;
		m_vPos = eEvent.fPos;
	}
}

void CElevator::Action_Trigger(const _float& fTimeDelta)
{
	auto pObj = m_pParent.lock();
	auto pDestTransform = CGameInstance::Get().Get_ObjectPtr(m_iLevel, L"Layer_Player", "Player")->Get_Transform().lock();
	if (NULL_TRUE(pObj))
		return;
	auto pSrcTransform = pObj->Get_Transform().lock();
	
	if (NULL_TRUE(pSrcTransform))
		return;

	_vector SrcPos = pSrcTransform->Get_State(STATE::POS);
	_float SrcHeight = XMVectorGetY(SrcPos);
	
	SrcHeight +=17.f * fTimeDelta;
	
	pSrcTransform->Set_State(STATE::POS,XMVectorSetY(SrcPos,SrcHeight));
	IS_PLAYSOUND(ELEVATOR_LOOP, CHANNELID::SOUND_OBJECT, 0.7f);
	if (SrcHeight >= 455.f)
	{
		STOP_SOUND(CHANNELID::SOUND_OBJECT);
		PLAY_SOUND(ELEVATOR_FINISH, CHANNELID::SOUND_OBJECT,0.7f);
		m_eState = TRIGGER_STATE::PAUSE;
		
		auto pObj = CGameInstance::Get().Get_ObjectPtr(m_iLevel, L"Layer_Player", "Player");
		auto pNavi = static_pointer_cast<CNavigation>(pObj->Find_Component(L"Com_Navigation"));

		pNavi->ReSearchCell(pDestTransform->Get_State(STATE::POS));
	}

}

void CElevator::Pause_Trigger()
{
	auto TriggerCheck = CGameInstance::Get().Find_Trigger(m_iLevel, m_iTargetNumber).lock();
	if (NULL_TRUE(TriggerCheck))
		return;

	TriggerCheck->TriggerToTrigger();
	m_eState = TRIGGER_STATE::END;
}

void CElevator::Idle_Trigger()
{
	auto pObj = m_pParent.lock();
	auto pDestTransform = m_pDstTransform.lock();
	if (NULL_TRUE(pObj) || NULL_TRUE(pDestTransform))
		return;
	auto pSrcTransform = pObj->Get_Transform().lock();

	if (NULL_TRUE(pSrcTransform))
		return;

	_vector SrcPos = pSrcTransform->Get_State(STATE::POS);
	_vector DestPos = pDestTransform->Get_State(STATE::POS);
	_float SrcHeight = XMVectorGetY(SrcPos);

	pDestTransform->Set_State(STATE::POS, XMVectorSetY(DestPos, SrcHeight));
}



unique_ptr<CElevator>CElevator::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CElevator>(new CElevator(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CElevator");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CElevator::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CElevator>(new CElevator(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CElevator Clone");
		return nullptr;
	}

	return pInstance;

}