#include "Player_FSM.h"

CPlayer_FSM::CPlayer_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CFSM_STATE(pDevice, pContext)
{
}

CPlayer_FSM::CPlayer_FSM(const CPlayer_FSM& Prototype) :CFSM_STATE(Prototype)
{
}

CPlayer_FSM::~CPlayer_FSM()
{
}

HRESULT CPlayer_FSM::Initialize_State(weak_ptr<CGameObject> pObj)
{
	auto Owner = pObj.lock();
	if(NULL_TRUE(Owner))
		return E_FAIL;

	m_pPlayer = static_pointer_cast<CPlayer>(Owner);

	return S_OK;
}