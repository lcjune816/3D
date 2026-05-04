#pragma once
#include "Component.h"


NS_BEGIN(Engine)
class ENGINE_DLL CFSM_STATE  abstract : public CComponent
{

protected:
	CFSM_STATE(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_STATE(const CFSM_STATE& Prototype);
public:
	virtual ~CFSM_STATE();

public:
	virtual HRESULT Initialize_State(weak_ptr<class CGameObject> pObj);
	virtual void Enter_State() PURE;
	virtual void Update_State(_float fTimeDelta)PURE;
	virtual void Exit_State()  PURE;

	void		Set_Machine(weak_ptr<class CFSM_Machine> pMachine);
protected:
	weak_ptr<class CFSM_Machine>		m_pMachine;

	_bool								m_bReFinished;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};


NS_END;