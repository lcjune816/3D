#include "FSM_Jump.h"

CFSM_Jump::CFSM_Jump(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_Jump::CFSM_Jump(const CFSM_Jump& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Jump::~CFSM_Jump()
{
}

void CFSM_Jump::Enter_State()
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player))return;

	Player->Change_Animation(PLAYER_ANIME::JUMP, false);

	m_fMaxHeight = 25.f;
	m_fJumpCnt = 0.f;
	m_fJumpSpeed =30.f;
	m_LastHeight = XMVectorGetY(Player->Get_Transform().lock()->Get_State(STATE::POS));
	m_fCurrentHeight = XMVectorGetY(Player->Get_Transform().lock()->Get_State(STATE::POS));
	m_eAction = FSM_ACTION::ACTION;
}

void CFSM_Jump::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	if (NULL_TRUE(Player)) return;
	if (NULL_TRUE(pTransform)) return;

	MOVE eMove = Player->Get_State();

	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;
	case FSM_ACTION::ACTION:
		Action_Jump(Player, pTransform,fTimeDelta);
		break;
	case FSM_ACTION::RETURN:
		Action_Return(Player, pTransform,fTimeDelta);
		break;
	case FSM_ACTION::END:
		Action_End(Player, pTransform);
		break;
	}

	auto pNavi = static_pointer_cast<CNavigation>(Player->Find_Component(L"Com_Navigation"));
	Move(fTimeDelta,  pTransform, pNavi);
}

void CFSM_Jump::Exit_State()
{
	auto Player = m_pPlayer.lock();
	Player->Set_Flag(ETOUI(PLAYER_FLAG::JUMP), FLAGVALUE::DISABLE);
}

void CFSM_Jump::Action_Jump(shared_ptr<CPlayer> pPlayer, shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	m_fJumpSpeed += 4.8f * fTimeDelta;
	m_fCurrentHeight += m_fJumpSpeed * fTimeDelta;
	
	if (m_fCurrentHeight >= m_fMaxHeight)
	{
		pPlayer->Change_Animation(PLAYER_ANIME::FALLING);
		m_eAction = FSM_ACTION::RETURN;
		m_fCurrentHeight = XMVectorGetY(pTransform->Get_State(STATE::POS));
	}
	pTransform->Set_State(STATE::POS, XMVectorSetY(pTransform->Get_State(STATE::POS), m_fCurrentHeight));
}

void CFSM_Jump::Action_Return(shared_ptr<CPlayer> pPlayer, shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{

	m_fJumpSpeed += -150.f* fTimeDelta;
	m_fCurrentHeight += m_fJumpSpeed * fTimeDelta;

	if (m_fCurrentHeight <= m_LastHeight)
	{

		pPlayer->GetAnimator()->Set_Double_Speed(1.5f);
		pPlayer->Change_Animation(PLAYER_ANIME::LAND,false , true,false);
		m_eAction = FSM_ACTION::END;
	}
	pTransform->Set_State(STATE::POS, XMVectorSetY(pTransform->Get_State(STATE::POS), m_fCurrentHeight));

}



void CFSM_Jump::Action_End(shared_ptr<CPlayer> pPlayer ,shared_ptr<class CTransform>pTransform )
{
	auto Machine = m_pMachine.lock();
	if (NULL_TRUE(Machine))
		return;
	if (pPlayer->Get_Finished())
	{
		auto& PlayerState = pPlayer->Get_AnimeState();
		pPlayer->GetAnimator()->Set_Double_Speed(1.f);
		Machine->Change_State(FSM::IDLE);
		return;
	}else
		pTransform->Set_State(STATE::POS, XMVectorSetY(pTransform->Get_State(STATE::POS), m_LastHeight));
}

unique_ptr<CFSM_Jump>		CFSM_Jump::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Jump>(new CFSM_Jump(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Jump::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Jump>(new CFSM_Jump(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}