#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_Idle : public CPlayer_FSM
{
private:
	CFSM_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Idle(const CFSM_Idle& Prototype);
public:
	virtual ~CFSM_Idle();

public:
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;


public:
	static unique_ptr<CFSM_Idle> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;

