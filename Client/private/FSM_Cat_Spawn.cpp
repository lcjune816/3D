#include "FSM_Cat_Spawn.h"

CFSM_Cat_Spawn::CFSM_Cat_Spawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CCat_FSM(pDevice, pContext)
{
}

CFSM_Cat_Spawn::CFSM_Cat_Spawn(const CFSM_Cat_Spawn& Prototype) :CCat_FSM(Prototype)
{
}

CFSM_Cat_Spawn::~CFSM_Cat_Spawn()
{
}
HRESULT CFSM_Cat_Spawn::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	auto pDesc = static_cast<TEACHER_SPAWN_DESC*>(pArg);

	//126

	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_SPAWN, SHARED_THIS(CFSM_Cat_Spawn));
	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_EVENT1, SHARED_THIS(CFSM_Cat_Spawn));
	return S_OK;
}
void CFSM_Cat_Spawn::Enter_State()
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	pBoss->Change_Animation(CAT_ANIME::DOORWAY,false);
	
	m_eAction = FSM_ACTION::IDLE;
}

void CFSM_Cat_Spawn::Update_State(_float fTimeDelta)
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss)) return;

	auto pTransform = pBoss->Get_Transform().lock();
	if (NULL_TRUE(pTransform)) return;

	auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
	if (NULL_TRUE(pNavi)) return;

	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;

	case FSM_ACTION::ACTION:
		Action(pBoss,pTransform, pNavi, fTimeDelta);
		break;

	case FSM_ACTION::RETURN:
		Action_Return(pBoss);
		break;

	case FSM_ACTION::EVENT:
		break;
	}
}

void CFSM_Cat_Spawn::Exit_State()
{

}


void CFSM_Cat_Spawn::Action(shared_ptr<CBoss_Cat> pBoss,shared_ptr<CTransform> pTransform, shared_ptr<CNavigation> pNavigation, const _float& fTimeDelta)
{
	
	if(pBoss->Get_Finished())
		m_eAction = FSM_ACTION::RETURN;
	
	pTransform->Go_Straight(fTimeDelta * fTimeDelta, pNavigation);
}

void CFSM_Cat_Spawn::Action_Return(shared_ptr<CBoss_Cat> pBoss)
{

	auto pMachine = m_pMachine.lock();
	pMachine->Change_State(FSM::MOVE);
}

void CFSM_Cat_Spawn::OnNotify(const EVENT& eEvent)
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	auto pTransform = pBoss->Get_Transform().lock();
	if (NULL_TRUE(pTransform))
		return;

	if (eEvent.eEvent == WORLD_EVENT::BOSS_SPAWN)
	{		
		STOP_SOUND(CHANNELID::SOUND_BGM01);
		PLAY_SOUND(CAT_NIGHTMAREBGM, CHANNELID::SOUND_BGM01, 0.15f);
		PLAY_SOUND(CAT_GAS,CHANNELID::SOUND_BOSS_EFFECT, 0.5f);
		m_eAction = FSM_ACTION::ACTION;
		auto pAnimator = static_pointer_cast<CAnimator>(pBoss->Find_Component(L"Com_Animator_Normal"));
		auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
		pAnimator->Stop_Animation(false);
	}
	else if (eEvent.eEvent == WORLD_EVENT::BOSS_EVENT1)
	{
		m_eAction = FSM_ACTION::RETURN;
	}
}
unique_ptr<CFSM_Cat_Spawn>		CFSM_Cat_Spawn::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Cat_Spawn>(new CFSM_Cat_Spawn(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Cat_Spawn::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Cat_Spawn>(new CFSM_Cat_Spawn(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}