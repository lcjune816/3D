#pragma once
#include "Player.h"

enum class FSM_HAND_FLAG { ATTACHED = 0x00000001, R = 0x00000002, PULL = 0x00000004, PULLEND = 0x00000008, WALLCOLLIDE = 0x00000010, TIMER = 0x00000020 ,
	END = 0xffffffff };

enum class CHANGE_STATE { ATTACHED_LONG, ATTACHED_SHORT, PULL, END };
NS_BEGIN(Engine)
class  CPlayer_FSM : public CFSM_STATE
{
public:
	typedef struct strfsmPlayer
	{
		_float4x4* ParentsMatrix;
	}FSM_PLAYER_DESC;

protected:
	CPlayer_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_FSM(const CPlayer_FSM& Prototype);
public:
	virtual ~CPlayer_FSM();

public:
	virtual HRESULT			Initialize_State(weak_ptr<CGameObject> pObj) override;
	virtual void			Enter_State() PURE;
	virtual void			Update_State(_float fTimeDelta)PURE;
	virtual void			Exit_State()  PURE;
	void					Set_Flag(uint32_t eState, FLAGVALUE eValue);
	_bool					Flag_Check(uint32_t iFlag);
	void					Timer(const _float& fTimeDelta);
protected:
	virtual void			Hand_State_Chand(CHANGE_STATE eChange) {};

public:
	void					Set_Player(weak_ptr<CPlayer> pPlayer) { m_pPlayer = pPlayer; }
protected:

	uint32_t							m_iStateFlag{ 0 };
	weak_ptr<CPlayer>					m_pPlayer;
	_float m_fShootMaxTime{}, m_fShootTime{}, m_fShootTimeTick{}, m_fSpeed{}, m_fBackShootTime{}, m_fBackShootTick{}, m_fTimerTick{}, m_fTimerTime{};

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};


NS_END;