#pragma once
#include "FSM_STATE.h"

NS_BEGIN(Engine)
class ENGINE_DLL CFSM_Machine : public CComponent
{
protected:
	CFSM_Machine(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Machine(const CFSM_Machine& Prototype);
public:
	virtual ~CFSM_Machine();

public:
	virtual			HRESULT Initialize_Prototype();
	virtual			HRESULT Initialize(void* pArg);

	void			Enter_Machine();
	void			Update_Machine(_float fTimeDelta);
	void			Exit_Machine();
	
	void			Change_State(FSM eMachineState);
	void			Set_Owner(weak_ptr<class CGameObject> pObj);

	
	weak_ptr<class CGameObject>		Get_Owner();
public:
	HRESULT			Add_State(FSM StateName ,shared_ptr<CFSM_STATE> pMachine);
private:
	map<FSM, shared_ptr<class CFSM_STATE>>			m_States;

	weak_ptr<class CFSM_STATE>				 m_pCurrentState;

	weak_ptr<class CGameObject>				 m_pOwnerObject;

private:
	FSM										m_eState;
	_bool									m_bCheckFinish;
public:

	static unique_ptr<CFSM_Machine>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);


};
NS_END

