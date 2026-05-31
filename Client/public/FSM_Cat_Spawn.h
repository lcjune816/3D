#pragma once
#include "Cat_FSM.h"

NS_BEGIN(Client)
class  CFSM_Cat_Spawn : public CCat_FSM, public CObserver
{
	typedef struct tagTeacherSpawndesc
	{
		uint32_t		iNaviEventIndex;
	}TEACHER_SPAWN_DESC;

private:
	CFSM_Cat_Spawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Cat_Spawn(const CFSM_Cat_Spawn& Prototype);
public:
	virtual ~CFSM_Cat_Spawn();

public:

	virtual HRESULT Initialize(void* pArg)override;
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

	virtual void OnNotify(const EVENT& eEvent) override;

private:
	uint32_t		m_iNaviEventIndex{};
	_float3			m_DestPos;
public:
	static unique_ptr<CFSM_Cat_Spawn> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;

