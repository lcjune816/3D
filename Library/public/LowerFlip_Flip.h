#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLowerFlip_Flip final : public CTrigger
{
protected:
	CLowerFlip_Flip(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLowerFlip_Flip(const CLowerFlip_Flip& Prototype);
public:
	virtual ~CLowerFlip_Flip();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual void	Set_Trigger() override;

private:
	void			Action_Trigger();

public:
	static unique_ptr<CLowerFlip_Flip>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END