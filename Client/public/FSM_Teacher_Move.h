#pragma once
#include "Teacher_FSM.h"

NS_BEGIN(Client)
class  CFSM_Teacher_Move : public CTeacher_FSM , public CObserver
{

private:
	CFSM_Teacher_Move(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Teacher_Move(const CFSM_Teacher_Move& Prototype);
public:
	virtual ~CFSM_Teacher_Move();

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;
	virtual void OnNotify(const EVENT& eEvent) override;


private:
	void Action_Change(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform>pTransform);
	void Action_Chase(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta);
	void Return_StopMove();
	void Boss_Tp(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta);
private:
	_float3			m_fPos{};
	int32_t			m_iIndex{};
	_bool			m_bStop{ true };
public:
	static unique_ptr<CFSM_Teacher_Move> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;