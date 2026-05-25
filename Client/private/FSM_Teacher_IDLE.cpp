#include "FSM_Teacher_IDLE.h"
CFSM_Teacher_IDLE::CFSM_Teacher_IDLE(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CTeacher_FSM(pDevice, pContext)
{
}

CFSM_Teacher_IDLE::CFSM_Teacher_IDLE(const CFSM_Teacher_IDLE& Prototype) :CTeacher_FSM(Prototype)
{
}

CFSM_Teacher_IDLE::~CFSM_Teacher_IDLE()
{
}
HRESULT CFSM_Teacher_IDLE::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eAction = FSM_ACTION::IDLE;

	CGameInstance::Get().Add_Observers(WORLD_EVENT::TEACHER_SPAWN, SHARED_THIS(CFSM_Teacher_IDLE));
	return S_OK;
}
void CFSM_Teacher_IDLE::Enter_State()
{

	auto Boss = m_pBoss.lock();

	if (NULL_TRUE(Boss))return;

}

void CFSM_Teacher_IDLE::Update_State(_float fTimeDelta)
{

	switch (m_eAction)
	{
		case FSM_ACTION::IDLE:
			break;
		case FSM_ACTION::ACTION:
			Action(fTimeDelta);
			break;
		case FSM_ACTION::RETURN:
			break;
	}


}

void CFSM_Teacher_IDLE::Exit_State()
{
	auto Boss = m_pBoss.lock();

	if (NULL_TRUE(Boss))return;
}

void CFSM_Teacher_IDLE::OnNotify(const EVENT& eEvent)
{
	m_eAction = FSM_ACTION::ACTION;
}

void CFSM_Teacher_IDLE::Action(const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;

	if (m_fTick > 1.f)
	{
		m_fTick = 0.f;
		++m_fTimeCnt;
	}

	if (m_fTimeCnt >= 3.f)
	{
		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine))
			return;

		pMachine->Change_State(FSM::SPAWN);
	}
}

unique_ptr<CFSM_Teacher_IDLE>		CFSM_Teacher_IDLE::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Teacher_IDLE>(new CFSM_Teacher_IDLE(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Teacher_IDLE::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Teacher_IDLE>(new CFSM_Teacher_IDLE(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}