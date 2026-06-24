#include "FSM_Dead.h"

CFSM_Dead::CFSM_Dead(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_Dead::CFSM_Dead(const CFSM_Dead& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Dead::~CFSM_Dead()
{
}

HRESULT CFSM_Dead::Initialize(void* pArg)
{
	CGameInstance::Get().Add_Observers(WORLD_EVENT::PLAYER_DEAD, SHARED_THIS(CFSM_Dead));
	return S_OK;
}

void CFSM_Dead::Enter_State()
{
	auto Player = m_pPlayer.lock();
	if (NULL_TRUE(Player)) return;
	//hard coding..
	auto pTransform = Player->Get_TransformPtr();
	_vector vSrcPos = pTransform->Get_State(STATE::POS) - XMVectorSet(-8, 33, 15, 0);
	_vector Offset = XMVectorSet(9.45, 28.7, 2, 1);
	XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMVectorSet(1.9, 37, -4.4, 0) - Offset));

	_float3 fScale = pTransform->Get_Scaled();
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), XMLoadFloat3(&m_vTargetLook)));
	_vector vUp = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vTargetLook), vRight));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	pTransform->Set_State(STATE::RIGHT, vRight * fScale.x);
	pTransform->Set_State(STATE::UP, vUp * fScale.y);
	pTransform->Set_State(STATE::LOOK, vLook * fScale.z);

	pTransform->Apply_Rotation(XMVectorSet(0, 1, 0, 0), 30.f);

	pTransform->Apply_Rotation(XMVectorSet(1, 0, 0, 0), 10.f);

	pTransform->Apply_Rotation(XMVectorSet(0, 0, 1, 0), 5.f);
	pTransform->Set_State(STATE::POS, Offset);

	Player->GetAnimator()->Stop_Animation(true);
	XMStoreFloat3(&m_vOriginLook, XMVector3Normalize(Player->Get_TransformPtr()->Get_State(STATE::LOOK)));
	m_eAction = FSM_ACTION::IDLE;
	m_fTimeOffset = 0.03f;
	m_fAngleOffset = 2.3f;
	m_fFrameTickDead = 0.f;
	m_fRotation = _float3(0,1,1) ;

}

void CFSM_Dead::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pTransform = Player->Get_Transform().lock();
	if (NULL_TRUE(Player)) return;
	if (NULL_TRUE(pTransform)) return;

	m_fFrameTickDead += fTimeDelta;

	if (m_fFrameTickDead > 1.5f)
		m_eAction = FSM_ACTION::EVENT2;

	if (m_fFrameTickDead > 2.f)
	{
		EVENT eEvent;
		eEvent.eEvent = WORLD_EVENT::END;
		CGameInstance::Get().Notify(WORLD_EVENT::END, eEvent);
	}
		
	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		OffsetTime(fTimeDelta);
		break;
	case FSM_ACTION::ACTION:
		ShakingCam1(pTransform, fTimeDelta);
		break;
	case FSM_ACTION::RETURN:
		ShakingCam2(pTransform, fTimeDelta);
		break;

	case FSM_ACTION::EVENT:
		ShakingCam3(pTransform, fTimeDelta);
		break;
	case FSM_ACTION::EVENT2:
		GoingHead(pTransform, fTimeDelta);
		break;
	}

	
}

void CFSM_Dead::Exit_State()
{
}

void CFSM_Dead::OnNotify(const EVENT& eEvent)
{

}

void CFSM_Dead::OffsetTime(const _float& fTimeDelta)
{
	m_fTimerTick += fTimeDelta;
	
	if (m_fTimerTick > 0.8f)
	{
		m_fTimerTick = 0.f;
		m_eAction = FSM_ACTION::ACTION;
	}

}

void CFSM_Dead::ShakingCam1(shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	m_fTimerTick += fTimeDelta;
	_float fTime = m_fTimerTick / m_fTimeOffset;

	_float fAngle = 0 - (m_fAngleOffset + 0) * fTime;


	pTransform->Apply_Rotation(XMLoadFloat3(&m_fRotation), fAngle);

	if (fTime >= 1.f)
	{
		m_bShaking = true;
		m_fAngle = m_fAngleOffset;
		m_fTimerTick = 0.f;
		m_eAction = FSM_ACTION::RETURN;
	}
}

void CFSM_Dead::ShakingCam2(shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	m_fTimerTick += fTimeDelta;
	_float fTime = m_fTimerTick / m_fTimeOffset;

	_float fAngle = m_fAngle - ( 0+ m_fAngle) * fTime;


	pTransform->Apply_Rotation(XMLoadFloat3(&m_fRotation), fAngle);

	if (fTime >= 1.f)
	{
		m_fTimerTick = 0.f;
		if(m_bShaking)
			m_eAction = FSM_ACTION::EVENT;
		else
			m_eAction = FSM_ACTION::ACTION;
	}
}

void CFSM_Dead::ShakingCam3(shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	m_fTimerTick += fTimeDelta;
	_float fTime = m_fTimerTick / m_fTimeOffset;

	_float fAngle = 0 - (-m_fAngleOffset + 0) * fTime;


	pTransform->Apply_Rotation(XMLoadFloat3(&m_fRotation), fAngle);

	if (fTime >= 1.f)
	{
		m_fAngle = -m_fAngleOffset;
		m_fTimerTick = 0.f;
		m_bShaking = false;
		m_eAction = FSM_ACTION::RETURN;
	}
}

void CFSM_Dead::GoingHead(shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	pTransform->Go_Straight(fTimeDelta, nullptr);
}

unique_ptr<CFSM_Dead>		CFSM_Dead::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Dead>(new CFSM_Dead(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Dead::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CFSM_Dead>(new CFSM_Dead(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}