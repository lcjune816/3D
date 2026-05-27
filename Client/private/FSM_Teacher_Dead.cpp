#include "FSM_Teacher_Dead.h"

CFSM_Teacher_Daed::CFSM_Teacher_Daed(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CTeacher_FSM(pDevice, pContext)
{
}

CFSM_Teacher_Daed::CFSM_Teacher_Daed(const CFSM_Teacher_Daed& Prototype) :CTeacher_FSM(Prototype)
{
}

CFSM_Teacher_Daed::~CFSM_Teacher_Daed()
{
}
HRESULT CFSM_Teacher_Daed::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}
void CFSM_Teacher_Daed::Enter_State()
{
	m_eAction = FSM_ACTION::IDLE;
	auto Boss = m_pBoss.lock();
	if (NULL_TRUE(Boss)) return;
	//Boss->GetAnimator()->Set_RootNode(false);
	Boss->Change_Animation(TEACHER_ANIME::DEAD_FAST,false);
	Boss->GetAnimator()->Set_RootNode(false);
}

void CFSM_Teacher_Daed::Update_State(_float fTimeDelta)
{

	auto Boss = m_pBoss.lock();
	auto pTransform = Boss->Get_Transform().lock();
	if (NULL_TRUE(Boss)) return;
	if (NULL_TRUE(pTransform)) return;

	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		Action_Chase(Boss, pTransform, fTimeDelta);
		break;

	case FSM_ACTION::ACTION:
		break;
	case FSM_ACTION::RETURN:
		Daed(Boss, pTransform, fTimeDelta);
		break;
	}

	
}

void CFSM_Teacher_Daed::Exit_State()
{

}

void CFSM_Teacher_Daed::OnNotify(const EVENT& eEvent)
{ 
	m_fWorldTime = static_cast<_float*>(eEvent.pArg);;
	m_eAction = FSM_ACTION::ACTION;

}

void CFSM_Teacher_Daed::Action_Chase(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta)
{
	//auto pNavi = static_pointer_cast<CNavigation>(pBoss->Find_Component(L"Com_Navigation"));
	//pTransform->MoveToAstar(pNavi, ETOUI(LEVEL::GAMEPLAY), L"Layer_Player", "Player", fTimeDelta);

	if (pBoss->Get_Finished())
		pBoss->GetAnimator()->Stop_Animation(true);


}

void CFSM_Teacher_Daed::Change_Dead(shared_ptr<CBoss_Teacher> pBoss)
{
}

void CFSM_Teacher_Daed::Daed(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta)
{
	//문 닫히는 남은거리
	//마지막 위치 
}



unique_ptr<CFSM_Teacher_Daed>		CFSM_Teacher_Daed::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Teacher_Daed>(new CFSM_Teacher_Daed(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Teacher_Daed::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Teacher_Daed>(new CFSM_Teacher_Daed(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}