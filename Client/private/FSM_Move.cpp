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

	if (CGameInstance::Get().Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		Player->GetAnimator()->Set_Double_Speed(2.5f);
		m_fVelocity = 2.f;
	}
	else
	{
		Player->GetAnimator()->Set_Double_Speed(1.f);
		m_fVelocity = 1.f;

	}

	auto pNavi = static_pointer_cast<CNavigation>(Player->Find_Component(L"Com_Navigation"));
	if (!Move(fTimeDelta, pTransform, pNavi, m_fVelocity))
	{
		auto machine = m_pMachine.lock();
		if (NULL_TRUE(machine)) return;
		machine->Change_State(FSM::IDLE);
		return;
	}
	
	if (CGameInstance::Get().Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_pMachine.lock()->Change_State(FSM::JUMP);
		Player->Set_Flag(ETOUI(PLAYER_FLAG::JUMP), FLAGVALUE::ENABLE);
	}
	if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		m_pMachine.lock()->Change_State(FSM::CROUCH);
		Player->Set_Flag(ETOUI(PLAYER_FLAG::CROUCH), FLAGVALUE::ENABLE);
	}
}

void CFSM_Move::Exit_State()
{
	auto Player = m_pPlayer.lock();
	Player->Set_Flag(ETOUI(PLAYER_FLAG::MOVE), FLAGVALUE::DISABLE);
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