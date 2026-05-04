#pragma once
#include "Player.h"

NS_BEGIN(Engine)
class  CPlayer_FSM : public CFSM_STATE
{

protected:
	CPlayer_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_FSM(const CPlayer_FSM& Prototype);
public:
	virtual ~CPlayer_FSM();

public:
	virtual HRESULT Initialize_State(weak_ptr<CGameObject> pObj) override;
	virtual void	 Enter_State() PURE;
	virtual void	 Update_State(_float fTimeDelta)PURE;
	virtual void	 Exit_State()  PURE;


public:
	void					Set_Player(weak_ptr<CPlayer> pPlayer) { m_pPlayer = pPlayer; }
protected:
	weak_ptr<CPlayer>				m_pPlayer;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};


NS_END;