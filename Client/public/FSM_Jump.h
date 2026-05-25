#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_Jump final : public CPlayer_FSM
{
private:
	CFSM_Jump(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Jump(const CFSM_Jump& Prototype);
public:
	virtual ~CFSM_Jump();

public:
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

private:
	void			Action_Jump(shared_ptr<CPlayer> pPlayer,   shared_ptr<class CTransform>pTransform, const _float& fTimeDelta);
	void			Action_Return(shared_ptr<CPlayer> pPlayer, shared_ptr<class CTransform>pTransform, const _float& fTimeDelta);
	void			Action_End(shared_ptr<CPlayer> pPlayer, shared_ptr<class CTransform>pTransform);
private:

	_float				m_LastHeight{};
	_float				m_fJumpCnt  {};
	_float				m_fJumpSpeed{};	
	_float				m_fCurrentHeight{};
	_float				m_fMaxHeight{};
public:
	static unique_ptr<CFSM_Jump> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;