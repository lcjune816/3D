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
	
	Player->Change_Animation(PLAYER_ANIME::CROUCH_ENTER, false);
		
	m_fTimerTick = 0.f;
	m_fTimerTime = 0.2f;
	m_fMaxCrouch = 10;
	m_fMinCrouch = 0;
	m_fCurrentHeight = { 0 };
	m_bReFinished = false;
	m_bhmm = false;
	m_eAction = FSM_ACTION::ACTION;
}

void CFSM_Crouch::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	if (NULL_TRUE(Player)) return;
	
	MOVE eMove = Player->Get_State();
	_bool bCrouch = false;


	m_fTimerTick += fTimeDelta;

	_float t = min(1.f, m_fTimerTick / m_fTimerTime);
	
	_float Height{};
	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
			break;
	case FSM_ACTION::ACTION:
		if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) & 0x80)
		{
			Height = m_fMinCrouch + (m_fMaxCrouch - m_fMinCrouch) * t;
			Player->Set_OffsetY(-Height);

			if (Player->Get_Finished() && Player->Get_Animation_State() != PLAYER_ANIME::CROUCH_POSE)
				Player->Change_Animation(PLAYER_ANIME::CROUCH_POSE, true);



		}
		else
		{
			m_fMaxCrouch = Player->Get_OffsetY();
			m_fTimerTick = 0.f;
			m_eAction = FSM_ACTION::RETURN;
			m_fTimerTime = 1.f;

			Player->GetAnimator()->Set_Double_Speed(1.5f);
			Player->Change_Animation(PLAYER_ANIME::CROUCH_EXIT, false,false,false);

		}
		break;
	case FSM_ACTION::RETURN:
		Height = m_fMaxCrouch + (m_fMinCrouch - m_fMaxCrouch) * t;
		Player->Set_OffsetY(Height);

		if (Player->Get_Finished() && t >= 1.f)
		{
			Player->Set_OffsetY(0);
			Player->GetAnimator()->Set_Double_Speed(1.f);
			m_eAction = FSM_ACTION::END;
		}
		break;

	case FSM_ACTION::END:
		
			auto machine = m_pMachine.lock();
			if (NULL_TRUE(machine)) return;
			machine->Change_State(FSM::IDLE);

			return;
		
		break;
	}

	auto pNavi = static_pointer_cast<CNavigation>(Player->Find_Component(L"Com_Navigation"));
	Move(fTimeDelta, pTransform, pNavi,0.5f);
}

void CFSM_Crouch::Exit_State()
{
	auto Player = m_pPlayer.lock();
	Player->Set_Flag(ETOUI(PLAYER_FLAG::CROUCH), FLAGVALUE::DISABLE);
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