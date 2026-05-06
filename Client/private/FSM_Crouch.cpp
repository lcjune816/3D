#include "FSM_Crouch.h"

CFSM_Crouch::CFSM_Crouch(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_Crouch::CFSM_Crouch(const CFSM_Crouch& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Crouch::~CFSM_Crouch()
{
}

void CFSM_Crouch::Enter_State()
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player))return;
	
	_bool bCrouch = Player->Get_AnimeState().bCrouch;

	if(bCrouch)
		Player->Change_Animation(PLAYER_ANIME::CROUCH_ENTER, false);
		
	m_bReFinished = true;
}

void CFSM_Crouch::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	if (NULL_TRUE(Player)) return;
	
	MOVE eMove = Player->Get_State();
	_bool bCrouch = Player->Get_AnimeState().bCrouch;
	if (!bCrouch)
	{
		if (!m_bReFinished)
		{
			Player->Change_Animation(PLAYER_ANIME::CROUCH_EXIT, false);
			m_bReFinished = true;
		}

		if (Player->Get_Finished())
		{
			auto machine = m_pMachine.lock();
			if (NULL_TRUE(machine)) return;
			machine->Change_State(FSM::IDLE);
			return;
		}
		return;
	}
	
	if (Player->Get_Finished() && Player->Get_Animation_State() != PLAYER_ANIME::CROUCH_POSE)
			Player->Change_Animation(PLAYER_ANIME::CROUCH_POSE, true);

	switch (eMove)
	{
	case MOVE::RIGHT:
		pTransform->Go_Right(fTimeDelta * 0.5f);
		break;
	case MOVE::FORWARD:
		pTransform->Go_Straight(fTimeDelta * 0.5f);
		break;
	case MOVE::LEFT:
		pTransform->Go_Left(fTimeDelta * 0.5f);
		break;
	case MOVE::BACKWARD:
		pTransform->Go_BackWard(fTimeDelta * 0.5f);
		break;
	}
}

void CFSM_Crouch::Exit_State()
{
	
}

unique_ptr<CFSM_Crouch>		CFSM_Crouch::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Crouch>(new CFSM_Crouch(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Crouch::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Crouch>(new CFSM_Crouch(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}