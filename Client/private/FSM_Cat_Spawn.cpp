#include "FSM_Cat_Spawn.h"

CFSM_Cat_Spawn::CFSM_Cat_Spawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CCat_FSM(pDevice, pContext)
{
}

CFSM_Cat_Spawn::CFSM_Cat_Spawn(const CFSM_Cat_Spawn& Prototype) :CCat_FSM(Prototype)
{
}

CFSM_Cat_Spawn::~CFSM_Cat_Spawn()
{
}
HRESULT CFSM_Cat_Spawn::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	auto pDesc = static_cast<TEACHER_SPAWN_DESC*>(pArg);

	//126

	CGameInstance::Get().Add_Observers(WORLD_EVENT::TEACHER_SPAWN, SHARED_THIS(CFSM_Cat_Spawn));
	return S_OK;
}
void CFSM_Cat_Spawn::Enter_State()
{
	auto Owner = m_pMachine.lock()->Get_Owner().lock();
	if (NULL_TRUE(Owner))
		return;

}

void CFSM_Cat_Spawn::Update_State(_float fTimeDelta)
{
	auto Owner = m_pMachine.lock()->Get_Owner().lock();
	if (NULL_TRUE(Owner))
		return;

	auto pTransform = Owner->Get_Transform().lock();
	auto pNavi = static_pointer_cast<CNavigation>(Owner->Find_Component(L"Com_Navigation"));

	if (NULL_TRUE(Owner) || NULL_TRUE(pTransform) || NULL_TRUE(pNavi)) return;



}

void CFSM_Cat_Spawn::Exit_State()
{

}


void CFSM_Cat_Spawn::OnNotify(const EVENT& eEvent)
{
	auto Pos = static_cast<_float3*>(eEvent.pArg);
	m_DestPos = *Pos;
}
unique_ptr<CFSM_Cat_Spawn>		CFSM_Cat_Spawn::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Cat_Spawn>(new CFSM_Cat_Spawn(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Cat_Spawn::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Cat_Spawn>(new CFSM_Cat_Spawn(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}