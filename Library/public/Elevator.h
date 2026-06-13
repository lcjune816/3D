#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CElevator final : public CTrigger
{
protected:
	CElevator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CElevator(const CElevator& Prototype);
public:
	virtual ~CElevator();

public:

	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false);

	virtual void					TriggerToTrigger()override;
	virtual void					Set_Trigger() override;

	

private:	
	void							Action_Trigger(const _float& fTimeDelta);
	void							Pause_Trigger();
	void							Idle_Trigger();
public:
	static unique_ptr<CElevator>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END