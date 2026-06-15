#include "FSM_Teacher_Spawn.h"

CFSM_Teacher_Spawn::CFSM_Teacher_Spawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CTeacher_FSM(pDevice, pContext)
{
}

CFSM_Teacher_Spawn::CFSM_Teacher_Spawn(const CFSM_Teacher_Spawn& Prototype) :CTeacher_FSM(Prototype)
{
}

CFSM_Teacher_Spawn::~CFSM_Teacher_Spawn()
{
}
HRESULT CFSM_Teacher_Spawn::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	auto pDesc = static_cast<TEACHER_SPAWN_DESC*>(pArg);

	m_iNaviEventIndex = pDesc->iNaviEventIndex;
	m_iNaviEventIndex = 118;
	//126

	CGameInstance::Get().Add_Observers(WORLD_EVENT::BOSS_SPAWN,SHARED_THIS(CFSM_Teacher_Spawn));
	return S_OK;
}
void CFSM_Teacher_Spawn::Enter_State()
{
	auto Boss = m_pBoss.lock();
	if (NULL_TRUE(Boss))
		return;

	Boss->Change_Animation(TEACHER_ANIME::DOORKICK, false);
	m_eTeacher = FSM_TEACHER::KICK;

}

void CFSM_Teacher_Spawn::Update_State(_float fTimeDelta)
{

	auto Boss = m_pBoss.lock();
	auto pTransform = Boss->Get_Transform().lock();
	auto pNavi = static_pointer_cast<CNavigation>(Boss->Find_Component(L"Com_Navigation"));

	if (NULL_TRUE(Boss) || NULL_TRUE(pTransform) || NULL_TRUE(pNavi)) return;

	switch (m_eTeacher)
	{
	case FSM_TEACHER::KICK:
		KickDoor(Boss,fTimeDelta);
		break;
	case FSM_TEACHER::SMASH:
		SMesh_Generator(Boss,pTransform,fTimeDelta);
		break;
	case FSM_TEACHER::CHASE:
		Chase(pNavi, Boss,pTransform, fTimeDelta);
		break;
	case FSM_TEACHER::TURN:
		Turn(Boss, pTransform);
		break;
	case FSM_TEACHER::END:
		StandBy(Boss, pTransform);
		break;
	}

}

void CFSM_Teacher_Spawn::Exit_State()
{

}

void CFSM_Teacher_Spawn::KickDoor(shared_ptr<CBoss_Teacher> pTeacher, const _float& fTimeDelta)
{
	if (pTeacher->Get_Finished())
	{
		m_eTeacher = FSM_TEACHER::CHASE;
		pTeacher->Change_Animation(TEACHER_ANIME::OVERWAL, true);
	}

	m_fTick += fTimeDelta;

	if (m_fTick > 0.1f)
	{
		++m_fTimeCnt;
		m_fTick = 0;
	}
	
	if (m_fTimeCnt == 3)
	{
		EVENT eEvent{};
		eEvent.eEvent = WORLD_EVENT::DOOR;
		CGameInstance::Get().Notify(WORLD_EVENT::DOOR, eEvent);
	}
	if (m_fTimeCnt == 4 && m_bOneSound == false)
	{
		PLAY_SOUND(TEACHER_BREAKDOOR, CHANNELID::SOUND_BOSS, 0.3f);
		m_bOneSound = true;
	}
		
}

void CFSM_Teacher_Spawn::Turn(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform> pTransform)
{
	auto pObj = CGameInstance::Get().Get_ObjectPtr(ETOUI(LEVEL::GAMEPLAY), L"Layer_Player", "Player");
	if (NULL_FALSE(pObj))
	{
		
		_vector DestPos = pObj->Get_Transform().lock()->Get_State(STATE::POS);
		_vector SrcPos = pTransform->Get_State(STATE::POS);
		_vector vTargetLook = XMVector3Normalize(DestPos - SrcPos);
		_vector vSrcLook = pTransform->Get_State(STATE::LOOK);

		_vector vLerpLook = XMVectorLerp(vSrcLook, vTargetLook, 0.5f);
			
		_float fDot = acosf(XMVectorGetX(XMVector3Dot(vSrcLook, vLerpLook)));

		pTransform->Apply_Rotation(XMVectorSet(0, 1, 0, 0), -fDot); //발전기랑 내적해서 구한 각도로	
		if (pBoss->Get_Finished())
		{
			m_eTeacher = FSM_TEACHER::END;
		}
	}
}

void CFSM_Teacher_Spawn::SMesh_Generator(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{

	m_fTick += fTimeDelta;
	if (m_fTick > 0.1f)
	{
		++m_fTimeCnt;
		m_fTick = 0.f;
	}

	if (m_fTimeCnt >= 11 && !m_bOneAction)
	{
		EVENT eEvent{};

		//발전기 부수는걸로변경 배터리 슝
		
		CGameInstance::Get().Notify(WORLD_EVENT::BATTERY, {});
		eEvent.eEvent = WORLD_EVENT::GENERATOR;
		CGameInstance::Get().Notify(WORLD_EVENT::GENERATOR, eEvent);

		eEvent.eEvent = WORLD_EVENT::BOSS_LIGHT_ON;
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_LIGHT_ON, eEvent);

		eEvent.eEvent = WORLD_EVENT::BOSS_LIGHT_OFF;
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_LIGHT_OFF, eEvent);
		m_bOneAction = true;
	}
	if (pBoss->Get_Finished())
	{
		pBoss->Change_Animation(TEACHER_ANIME::OVERWAL, false, true);
		m_eTeacher = FSM_TEACHER::TURN;
	}
}


void CFSM_Teacher_Spawn::StandBy(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform)
{
	auto pMachine = m_pMachine.lock();
	if (NULL_TRUE(pMachine))
		return;
	pMachine->Change_State(FSM::MOVE);
}

void CFSM_Teacher_Spawn::Chase(shared_ptr<CNavigation> pNavi, shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform, const _float& fTimeDelta)
{
	if (pTransform->Chase_NaviTarget(fTimeDelta, m_iNaviEventIndex, pNavi))
	{
		pBoss->Change_Animation(TEACHER_ANIME::SMASH, false);
		m_eTeacher = FSM_TEACHER::SMASH;

		m_fTick = 0.f;
		m_fTimeCnt = 0;
	}
		
	_vector SrcPos = pTransform->Get_State(STATE::POS);
	_vector DestPos = XMLoadFloat3(&m_DestPos);

	_vector vSrcLook = pTransform->Get_State(STATE::LOOK);
	_vector vDestLook = XMVector3Normalize(DestPos - SrcPos);

	_vector LerpLook = XMVectorLerp(vSrcLook, vDestLook, fTimeDelta);

	_float fDot = acosf(XMVectorGetX(XMVector3Dot(vSrcLook, LerpLook)));
	pTransform->Apply_Rotation(XMVectorSet(0, 1, 0, 0), fDot); //발전기랑 내적해서 구한 각도로		
	
}

void CFSM_Teacher_Spawn::OnNotify(const EVENT& eEvent)
{
	auto Pos = static_cast<_float3*>(eEvent.pArg);
	m_DestPos = *Pos;	
}
unique_ptr<CFSM_Teacher_Spawn>		CFSM_Teacher_Spawn::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Teacher_Spawn>(new CFSM_Teacher_Spawn(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Teacher_Spawn::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Teacher_Spawn>(new CFSM_Teacher_Spawn(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}