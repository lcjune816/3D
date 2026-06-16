#include "FSM_Cat_Nightmare.h"

CFSM_Cat_Nightmare::CFSM_Cat_Nightmare(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CCat_FSM(pDevice, pContext)
{
}

CFSM_Cat_Nightmare::CFSM_Cat_Nightmare(const CFSM_Cat_Nightmare& Prototype) :CCat_FSM(Prototype)
{
}

CFSM_Cat_Nightmare::~CFSM_Cat_Nightmare()
{
}
HRESULT CFSM_Cat_Nightmare::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	auto pDesc = static_cast<TEACHER_SPAWN_DESC*>(pArg);

	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_TP, SHARED_THIS(CFSM_Cat_Nightmare));
	m_eAction = FSM_ACTION::IDLE;
	return S_OK;
}
void CFSM_Cat_Nightmare::Enter_State()
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	pBoss->Change_Form(CATFORM::NIGHTMARE);
	pBoss->Change_Animation_Nightmare(CAT_ANIME_NIGHTMARE::WALK,true);
	
}

void CFSM_Cat_Nightmare::Update_State(_float fTimeDelta)
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss)) return;

	auto pTransform = pBoss->Get_Transform().lock();
	if (NULL_TRUE(pTransform)) return;

	auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
	if (NULL_TRUE(pNavi)) return;


	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;

	case FSM_ACTION::ACTION:
		Action(pTransform, pBoss, fTimeDelta);
		break;

	case FSM_ACTION::RETURN:
		Action_Return(pTransform, pBoss, fTimeDelta);
		break;

	case FSM_ACTION::EVENT:
		break;
	}
}

void CFSM_Cat_Nightmare::Exit_State()
{

}

void CFSM_Cat_Nightmare::Action(shared_ptr<CTransform> pTransform, shared_ptr<CBoss_Cat> pBoss, const _float& fTimeDelta)
{
	_vector vDestPos = XMLoadFloat3(&m_fDestPos);
	
	m_fTick += fTimeDelta;
	_float fTime = min(1.f,m_fTick / 2.f);
	
	_vector vLinearPos = XMVectorLerp(XMLoadFloat3(&m_fCurrentPos), vDestPos, fTime);
	pTransform->Set_State(STATE::POS, XMVectorSetW(vLinearPos,1.f));

	if (fTime >= 1.f)
	{
		m_fTick = 0.f;

		m_eAction = FSM_ACTION::RETURN;
	}
}

void CFSM_Cat_Nightmare::Action_Return(shared_ptr<CTransform> pTransform, shared_ptr<CBoss_Cat> pBoss, const _float& fTimeDelta)
{
	m_fTick += fTimeDelta;
	_float fTime = min(1.f, m_fTick / 1.f);


	_vector vLinearLook = XMVectorLerp(XMLoadFloat3(&m_fCurretLook), XMLoadFloat3(&m_fPreLook), fTime);

	XMStoreFloat3(&m_fCurretLook, vLinearLook);
	_vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLinearLook);
	_vector vUp = XMVector3Cross(vLinearLook, vRight);
	pTransform->Set_State(STATE::RIGHT, vRight);
	pTransform->Set_State(STATE::UP, vUp);
	pTransform->Set_State(STATE::LOOK, vLinearLook);

	if (fTime >= 1.f)
	{

		auto pAnimator = static_pointer_cast<CAnimator>(pBoss->Find_Component(L"Com_Animator_Nightmare"));
		pAnimator->Set_Double_Speed(1.f);
		pBoss->Change_Animation_Nightmare(CAT_ANIME_NIGHTMARE::ELEVEATOR, false, false);
		m_eAction = FSM_ACTION::IDLE;
	}
}

void CFSM_Cat_Nightmare::OnNotify(const EVENT& eEvent)
{
	auto pBoss = m_pBoss.lock();
	if (NULL_TRUE(pBoss))
		return;
	auto pTransform = pBoss->Get_Transform().lock();
	if (NULL_TRUE(pTransform))
		return;

	if (eEvent.eEvent == WORLD_EVENT::BOSS_TP)
	{
		auto pAnimator = static_pointer_cast<CAnimator>(pBoss->Find_Component(L"Com_Animator_Nightmare"));
		 //eEvent.eEvent 
		 //eEvent.iIndex 
		m_fDestPos = eEvent.fPos;
		pAnimator->Stop_Animation(false);
		pAnimator->Set_RootNode(true);
		pAnimator->Set_Double_Speed(5.f);
		m_eAction = FSM_ACTION::ACTION;
		XMStoreFloat3(&m_fPreLook, pTransform->Get_State(STATE::LOOK));
		XMStoreFloat3(&m_fCurrentPos, pTransform->Get_State(STATE::POS));

		_vector vLook = XMVector3Normalize(XMLoadFloat3(&m_fDestPos) - XMLoadFloat3(&m_fCurrentPos));
		XMStoreFloat3(&m_fCurretLook, vLook);
		_vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook);
		_vector vUp = XMVector3Cross(vLook, vRight);
		pBoss->Get_TransformPtr()->Set_State(STATE::RIGHT, vRight);
		pBoss->Get_TransformPtr()->Set_State(STATE::UP, vUp);
		pBoss->Get_TransformPtr()->Set_State(STATE::LOOK, vLook);

	}

}
unique_ptr<CFSM_Cat_Nightmare>		CFSM_Cat_Nightmare::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Cat_Nightmare>(new CFSM_Cat_Nightmare(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Cat_Nightmare::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Cat_Nightmare>(new CFSM_Cat_Nightmare(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}