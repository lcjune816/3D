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
	return S_OK;
}
void CFSM_Teacher_Move::Enter_State()
{
}

void CFSM_Teacher_Move::Update_State(_float fTimeDelta)
{

	auto Boss = m_pBoss.lock();
	auto pTransform = Boss->Get_Transform().lock();
	if (NULL_TRUE(Boss)) return;
	if (NULL_TRUE(pTransform)) return;

	MOVE eMove = Boss->Get_State();
	_bool bMove = Boss->Get_AnimeState().bMove;

	if (!bMove)
	{
		auto machine = m_pMachine.lock();
		if (NULL_TRUE(machine)) return;
		machine->Change_State(FSM::IDLE);
		return;
	}

	if (bMove)
	{
		auto pNavi = static_pointer_cast<CNavigation>(Boss->Find_Component(L"Com_Navigation"));
		pTransform->MoveToAstar(pNavi, ETOUI(LEVEL::GAMEPLAY), L"Layer_Player", "Player",fTimeDelta);

		if (Boss->Get_Animation_State() != TEACHER_ANIME::OVERSHOOTWALK)
			Boss->Change_Animation(TEACHER_ANIME::OVERSHOOTWALK, true);

	}

	
	
}

void CFSM_Teacher_Move::Exit_State()
{

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