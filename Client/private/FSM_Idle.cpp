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