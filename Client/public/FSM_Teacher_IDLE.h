#pragma once
#include "Teacher_FSM.h"

NS_BEGIN(Client)
class  CFSM_Teacher_IDLE : public CTeacher_FSM, public CObserver
{
private:
	CFSM_Teacher_IDLE(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Teacher_IDLE(const CFSM_Teacher_IDLE& Prototype);
public:
	virtual ~CFSM_Teacher_IDLE();

public:

	virtual HRESULT Initialize(void* pArg)override;
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;
	virtual void OnNotify(const EVENT& eEvent) override;

private:
	void Action(const _float& fTimeDelta);
public:
	static unique_ptr<CFSM_Teacher_IDLE> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;

