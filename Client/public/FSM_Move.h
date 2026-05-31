#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_Move final : public CPlayer_FSM
{
private:
	CFSM_Move(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Move(const CFSM_Move& Prototype);
public:
	virtual ~CFSM_Move();

public:
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

private:
	_float			m_fVelocity{};
public:
	static unique_ptr<CFSM_Move> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;