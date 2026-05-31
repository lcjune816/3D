#include "FSM_Teacher_Move.h"

CFSM_Teacher_Move::CFSM_Teacher_Move(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CTeacher_FSM(pDevice, pContext)
{
}

CFSM_Teacher_Move::CFSM_Teacher_Move(const CFSM_Teacher_Move& Prototype) :CTeacher_FSM(Prototype)
{
}

CFSM_Teacher_Move::~CFSM_Teacher_Move()
{
}
HRESULT CFSM_Teacher_Move::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	CGameInstance::Get().Add_Observers(WORLD_EVENT::TEACHER_DEAD, SHARED_THIS(CFSM_Teacher_Move));
	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_TP, SHARED_THIS(CFSM_Teacher_Move));
	return S_OK;
}
void CFSM_Teacher_Move::Enter_State()
{
	m_eAction = FSM_ACTION::RETURN;

}

void CFSM_Teacher_Move::Update_State(_float fTimeDelta)
{

	auto Boss = m_pBoss.lock();
	auto pTransform = Boss->Get_Transform().lock();
	if (NULL_TRUE(Boss)) return;
	if (NULL_TRUE(pTransform)) return;

	Action_Change(Boss, pTransform);
	switch(m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;

	case FSM_ACTION::ACTION:
		Action_Chase(Boss,pTransform,fTimeDelta);
		break;

	case FSM_ACTION::RETURN:
		if (Boss->Get_Finished())
			m_eAction = FSM_ACTION::ACTION;
		break;
	case FSM_ACTION::EVENT:
		Boss_Tp(Boss, pTransform, fTimeDelta);
		break;
	}


}

void CFSM_Teacher_Move::Exit_State()
{

}

void CFSM_Teacher_Move::OnNotify(const EVENT& eEvent)
{
	auto pMachine = m_pMachine.lock();
	if (NULL_TRUE(pMachine))
		return;
	
	if (eEvent.eEvent == WORLD_EVENT::BOSS_TP)
	{
		m_fPos = eEvent.fPos;
		m_iIndex = eEvent.iIndex;
		m_eAction = FSM_ACTION::EVENT;
		m_bStop = false;
	}else
		pMachine->Change_State(FSM::END);
}

void CFSM_Teacher_Move::Action_Change(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform>pTransform)
{
	if (!m_bStop)
		return;

	if (CGameInstance::Get().RayCast(ETOUI(LEVEL::END), L"Layer_WorldObject", L"Layer_Player", "Player", pTransform, XMVectorSet(0.f, 8.f, 0.f, 1.f)))
	{

		pBoss->GetAnimator()->Stop_Animation(true);
		m_eAction = FSM_ACTION::IDLE;
	}
	else
	{
		pBoss->GetAnimator()->Stop_Animation(false);
		m_eAction = FSM_ACTION::ACTION;
	}

}
void CFSM_Teacher_Move::Action_Chase(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta)
{
	auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
	pTransform->MoveToAstar(pNavi, ETOUI(LEVEL::GAMEPLAY), L"Layer_Player", "Player", fTimeDelta);

	if (pBoss->Get_Animation_State() != TEACHER_ANIME::OVERSHOOTWALK)
		pBoss->Change_Animation(TEACHER_ANIME::OVERSHOOTWALK, true);
}

void CFSM_Teacher_Move::Return_StopMove()
{
}

void CFSM_Teacher_Move::Boss_Tp(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta)
{
	
	m_fTick += fTimeDelta;
	if (m_fTick > 1.f)
	{
		m_fTick = 0.f;
		++m_fTimeCnt;
	}

	if (m_fTimeCnt >= 3.f)
	{
		auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
		pNavi->Set_CurrentIndex(m_iIndex);

		pBoss->Get_Transform().lock()->Set_State(STATE::POS, XMVectorSetW(XMLoadFloat3(&m_fPos), 1.f));
		m_eAction = FSM_ACTION::ACTION;
	
	}
	
}

unique_ptr<CFSM_Teacher_Move>		CFSM_Teacher_Move::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Teacher_Move>(new CFSM_Teacher_Move(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Teacher_Move::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Teacher_Move>(new CFSM_Teacher_Move(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}