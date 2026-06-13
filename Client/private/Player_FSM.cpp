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
_bool CPlayer_FSM::Flag_Check(uint32_t iFlag)
{
	if (m_iStateFlag & iFlag)
		return true;

	return false;
}
 
void CPlayer_FSM::Set_Flag(uint32_t eState, FLAGVALUE eValue)
{
	switch (eValue)
	{
	case FLAGVALUE::ENABLE:

		m_iStateFlag |= eState;
		break;

	case FLAGVALUE::DISABLE:

		m_iStateFlag &= ~eState;
		break;

	case FLAGVALUE::TOGGLE:

		m_iStateFlag ^= eState;
		break;

	case FLAGVALUE::RESET:

		m_iStateFlag = 0;
		break;

	}
}

void CPlayer_FSM::Timer(const _float& fTimeDelta)
{
	if (!Flag_Check(ETOUI(FSM_HAND_FLAG::TIMER)))
		return;

	m_fTimerTick += fTimeDelta;

	if (m_fTimerTick > 0.01f)
	{
		m_fTimerTick = 0;
		++m_fTimerTime;
	}

	if (m_fTimerTime > 30.f)
	{
		Hand_State_Chand(CHANGE_STATE::END);
	}

}

_bool CPlayer_FSM::Move(const _float& fTimeDelta, shared_ptr<CTransform>& pTransform, shared_ptr<class CNavigation>& pNavigation, _float fOffset)
{
	if (CGameInstance::Get().Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		IS_PLAYSOUND(PLAYER_SOUND_WALK,CHANNELID::PLAYER_MOVE, 0.4f)

		pTransform->Go_Right(fTimeDelta * fOffset, pNavigation,true);
		return true;
	}
	if (CGameInstance::Get().Get_DIKeyState(DIK_UP) & 0x80)
	{
		IS_PLAYSOUND(PLAYER_SOUND_WALK, CHANNELID::PLAYER_MOVE, 0.4f)

		pTransform->Go_Straight(fTimeDelta * fOffset, pNavigation, true);
		return true;
	}
		
	if (CGameInstance::Get().Get_DIKeyState(DIK_LEFT) & 0x80)
	{

		IS_PLAYSOUND(PLAYER_SOUND_WALK, CHANNELID::PLAYER_MOVE, 0.4f)
		pTransform->Go_Left(fTimeDelta * fOffset, pNavigation, true);
		return true;
	}
		
	if (CGameInstance::Get().Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		IS_PLAYSOUND(PLAYER_SOUND_WALK, CHANNELID::PLAYER_MOVE, 0.4f)
		pTransform->Go_BackWard(fTimeDelta * fOffset, pNavigation, true);
		return true;
	}
		

	return false;
}
