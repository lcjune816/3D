#include "GameInstance.h"
CFSM_Machine::CFSM_Machine(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext):CComponent{pDevice,pContext}
{ }
CFSM_Machine::CFSM_Machine(const CFSM_Machine& Prototype): CComponent{ Prototype }
{}
CFSM_Machine::~CFSM_Machine() {}

HRESULT CFSM_Machine::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CFSM_Machine::Initialize(void* pArg)
{

	return S_OK;
}

void CFSM_Machine::Enter_Machine()
{
	auto machine = m_pCurrentState.lock();

	if (NULL_FALSE(machine))
		machine->Enter_State();

}

void CFSM_Machine::Update_Machine(_float fTimeDelta)
{
	auto machine = m_pCurrentState.lock();

	if (NULL_FALSE(machine))
		machine->Update_State(fTimeDelta);


}

void CFSM_Machine::Exit_Machine()
{
	auto machine = m_pCurrentState.lock();

	if (NULL_FALSE(machine))
		machine->Exit_State();

}

void CFSM_Machine::Change_State(FSM eMachineState )
{
	if (m_eState == eMachineState) return;

	auto Owner = m_pOwnerObject.lock();
	if (NULL_TRUE(Owner)) return;
	Owner->Set_Finisihed(false);
	Exit_Machine();

	m_eState = eMachineState;

	auto machine = m_States.find(m_eState);

	if (machine != m_States.end())
	{
		m_pCurrentState = machine->second;
		m_pCurrentState.lock()->Enter_State();
		m_pCurrentState.lock()->Set_Machine(SHARED_THIS(CFSM_Machine));

	}
	else
	{
		m_pCurrentState.reset();
	}
}


void CFSM_Machine::Set_Owner(weak_ptr<CGameObject> pObj)
{
	m_pOwnerObject = pObj;

}

weak_ptr<class CGameObject> CFSM_Machine::Get_Owner()
{
	return m_pOwnerObject;
}

HRESULT			CFSM_Machine::Add_State(FSM StateName, shared_ptr<CFSM_STATE> pMachine)
{
	auto iter = m_States.find(StateName);

	if (iter == m_States.end())
	{
		pMachine->Initialize_State(m_pOwnerObject);

		m_States.emplace(StateName, pMachine);
	}
	
	return S_OK;
}

unique_ptr<CFSM_Machine>		CFSM_Machine::Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Machine>(new CFSM_Machine(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Machine::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Machine>(new CFSM_Machine(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}