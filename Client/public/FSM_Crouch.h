#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_Crouch final : public CPlayer_FSM
{
private:
	CFSM_Crouch(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Crouch(const CFSM_Crouch& Prototype);
public:
	virtual ~CFSM_Crouch();

public:
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;



private:
	_float			m_fMinCrouch{ 5.f }, m_fMaxCrouch{ 10.f }, m_fCurrentCrouch{}, m_fCrouchTime{ 2 };
	_float			m_fCurrentHeight{};
	_bool			m_bhmm{ false };
public:
	static unique_ptr<CFSM_Crouch> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;