#include "FSM_STATE.h"
#include "FSM_Machine.h"
CFSM_STATE::CFSM_STATE(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CComponent{pDevice,pContext}
{
}

CFSM_STATE::CFSM_STATE(const CFSM_STATE& Prototype):CComponent{Prototype}
{
}

CFSM_STATE::~CFSM_STATE()
{
}

HRESULT CFSM_STATE::Initialize_State(weak_ptr<CGameObject> pObj)
{
	return S_OK;
}

void CFSM_STATE::Set_Machine(weak_ptr<class CFSM_Machine> pMachine)
{
	m_pMachine = pMachine;
}
