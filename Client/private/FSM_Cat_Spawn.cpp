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

	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_SPAWN, SHARED_THIS(CFSM_Cat_Spawn));
	return S_OK;
}
void CFSM_Cat_Spawn::Enter_State()
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	pBoss->Change_Animation(CAT_ANIME::DOORWAY);

	m_eAction = FSM_ACTION::IDLE;
}

void CFSM_Cat_Spawn::Update_State(_float fTimeDelta)
{
	auto Owner = m_pMachine.lock()->Get_Owner().lock();
	if (NULL_TRUE(Owner)) return;

	auto pTransform = Owner->Get_Transform().lock();
	if (NULL_TRUE(pTransform)) return;
	
	auto pNavi = static_pointer_cast<CNavigation>(Owner->Find_Component(L"Com_Navigation"));
	if (NULL_TRUE(pNavi)) return;

	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;

	case FSM_ACTION::ACTION:
		//Action(pTransform, pNavi, fTimeDelta);
		break;

	case FSM_ACTION::RETURN:
		break;

	case FSM_ACTION::EVENT:
		break;
	}
}

void CFSM_Cat_Spawn::Exit_State()
{

}

void CFSM_Cat_Spawn::Action(shared_ptr<CTransform> pTransform, shared_ptr<CNavigation> pNavigation, const _float& fTimeDelta)
{
	pTransform->Go_Straight(fTimeDelta, pNavigation);
}

void CFSM_Cat_Spawn::OnNotify(const EVENT& eEvent)
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	auto pTransform = pBoss->Get_Transform().lock();
	if (NULL_TRUE(pTransform))
		return;

	if (eEvent.eEvent == WORLD_EVENT::BOSS_SPAWN)
	{		
		m_eAction = FSM_ACTION::ACTION;
		auto pAnimator = static_pointer_cast<CAnimator>(pBoss->Find_Component(L"Com_Animator_Normal"));
		auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
		pAnimator->Stop_Animation(false);

		_vector vSrcPos = pTransform->Get_State(STATE::POS);
		_vector vSrcLook = pTransform->Get_State(STATE::LOOK);
		_vector vDestPos = pNavi->Get_CellEventPos(CELL_EVENT::ARROW);

		_vector vTargetLook = XMVector3Normalize(vDestPos - vSrcPos);
		_float fAngle = XMVectorGetX(XMVector3Dot(vSrcLook, vTargetLook));

		//pTransform->Apply_Rotation(XMVectorSet(0,1,0,0), XMConvertToDegrees(fAngle));

	}

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