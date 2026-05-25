#include "FSM_Idle.h"
CFSM_Idle::CFSM_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice,pContext)
{
}

CFSM_Idle::CFSM_Idle(const CFSM_Idle& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Idle::~CFSM_Idle()
{
}

void CFSM_Idle::Enter_State()
{

	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player))return;

	Player->Change_Animation(PLAYER_ANIME::IDLE, true);
}

void CFSM_Idle::Update_State(_float fTimeDelta)
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player))return;
	
	
	if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		m_pMachine.lock()->Change_State(FSM::CROUCH);
		Player->Set_Flag(ETOUI(PLAYER_FLAG::CROUCH), FLAGVALUE::ENABLE);
	}

	if (CGameInstance::Get().Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_pMachine.lock()->Change_State(FSM::JUMP);
		Player->Set_Flag(ETOUI(PLAYER_FLAG::JUMP), FLAGVALUE::ENABLE);
	}

	if ((CGameInstance::Get().Get_DIKeyState(DIK_LEFT) & 0x80) || (CGameInstance::Get().Get_DIKeyState(DIK_RIGHT) & 0x80) || (CGameInstance::Get().Get_DIKeyState(DIK_UP) & 0x80) || (CGameInstance::Get().Get_DIKeyState(DIK_DOWN) & 0x80))
	{
		m_pMachine.lock()->Change_State(FSM::MOVE);
		Player->Set_Flag(ETOUI(PLAYER_FLAG::MOVE), FLAGVALUE::ENABLE);
	}

}

void CFSM_Idle::Exit_State()
{
}

unique_ptr<CFSM_Idle>		CFSM_Idle::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Idle>(new CFSM_Idle(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Idle::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Idle>(new CFSM_Idle(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}