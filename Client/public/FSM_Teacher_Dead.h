#pragma once
#include "Teacher_FSM.h"

NS_BEGIN(Client)
class  CFSM_Teacher_Daed : public CTeacher_FSM , public CObserver
{

private:
	CFSM_Teacher_Daed(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Teacher_Daed(const CFSM_Teacher_Daed& Prototype);
public:
	virtual ~CFSM_Teacher_Daed();

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

	virtual void OnNotify(const EVENT& eEvent) override;

private:
	void Action_Chase(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta);
	void Change_Dead(shared_ptr<CBoss_Teacher> pBoss, const _float& fTimeDelta);
	void Daed(shared_ptr<CBoss_Teacher> pBoss, shared_ptr<CTransform>pTransform, const _float& fTimeDelta);

private:
	_float	m_fTimeTick{};
	_float* m_fWorldTime;
public:
	static unique_ptr<CFSM_Teacher_Daed> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;