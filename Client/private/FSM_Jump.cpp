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

	m_bJump = true;
	m_bFalling = false;
	m_bReFinished = false;
	m_fJumpTick = 0.f;
	m_fDropTime = 0.f;
	m_fMaxHeight = 15.f;
	m_fJumpCnt = 0.f;
	m_fJumpSpeed = 50.f;
}

void CFSM_Jump::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	auto Machine = m_pMachine.lock();
	if (NULL_TRUE(Player)) return;
	if (NULL_TRUE(Machine)) return;
	if (NULL_TRUE(pTransform)) return;

	MOVE eMove = Player->Get_State();

	_float4 vPos{};
	XMStoreFloat4(&vPos, pTransform->Get_State(STATE::POS));
	if (!m_bJump && vPos.y <= 0.f)
	{
		vPos.y = 0.f;

		pTransform->Set_State(STATE::POS, XMLoadFloat4(&vPos));

		if (!m_bReFinished)
		{
			Player->Change_Animation(PLAYER_ANIME::LAND);
			m_bReFinished = true;

		}
		if (Player->Get_Finished())
		{
			auto& PlayerState = Player->Get_AnimeState();
			PlayerState.bFalling = false;
			PlayerState.bJump = false;
			Machine->Change_State(FSM::IDLE);
			return;
		}
		m_bFalling = false;
		return;
	}

	if (!m_bJump)
	{
		m_fDropTime += 9.8f * fTimeDelta;
		vPos.y -= m_fDropTime;
	}

	

	if(m_bJump)
	{
		m_fJumpTick += m_fJumpSpeed * fTimeDelta;

		vPos.y += m_fJumpTick * fTimeDelta;

		if (vPos.y >= m_fMaxHeight)
		{
			Player->Change_Animation(PLAYER_ANIME::FALLING);
			m_bJump = false;
		}
			
	}

	pTransform->Set_State( STATE::POS, XMLoadFloat4(&vPos));

	switch (eMove)
	{
	case MOVE::RIGHT:
		pTransform->Go_Right(fTimeDelta);
		break;
	case MOVE::FORWARD:
		pTransform->Go_Straight(fTimeDelta);
		break;
	case MOVE::LEFT:
		pTransform->Go_Left(fTimeDelta);
		break;
	case MOVE::BACKWARD:
		pTransform->Go_BackWard(fTimeDelta);
		break;
	}
}

void CFSM_Jump::Exit_State()
{
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