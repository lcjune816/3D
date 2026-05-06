#include "FSM_Move.h"

CFSM_Move::CFSM_Move(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_Move::CFSM_Move(const CFSM_Move& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Move::~CFSM_Move()
{
}

void CFSM_Move::Enter_State()
{
}

void CFSM_Move::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	if (NULL_TRUE(Player)) return;
	if (NULL_TRUE(pTransform)) return;

	MOVE eMove = Player->Get_State();
	_bool bRun = Player->Get_AnimeState().bRun;
	_bool bMove = Player->Get_AnimeState().bMove;
	
	if (!bMove)
	{
		auto machine = m_pMachine.lock();
		if (NULL_TRUE(machine)) return;
		machine->Change_State(FSM::IDLE);
		return;
	}
	
	if (bRun)
	{
		if(Player->Get_Animation_State() != PLAYER_ANIME::RUN)
			Player->Change_Animation(PLAYER_ANIME::RUN, true);
	}
	else
	{
		if(Player->Get_Animation_State() != PLAYER_ANIME::WALK)
			Player->Change_Animation(PLAYER_ANIME::WALK, true);
	}
		
	switch (eMove)
	{
	case MOVE::RIGHT:
		pTransform->Go_Right(fTimeDelta);
		break;
	case MOVE::FORWARD:
		pTransform->Go_Straight(fTimeDelta);
		break;
	case MOVE::LEFT:
		pTransform->Go_Left(fTimeDelta);
		break;
	case MOVE::BACKWARD:
		pTransform->Go_BackWard(fTimeDelta);
		break;
	}
}

void CFSM_Move::Exit_State()
{

}

unique_ptr<CFSM_Move>		CFSM_Move::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Move>(new CFSM_Move(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Move::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Move>(new CFSM_Move(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}