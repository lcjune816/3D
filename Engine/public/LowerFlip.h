#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLowerFlip final : public CTrigger
{
protected:
	CLowerFlip(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLowerFlip(const CLowerFlip& Prototype);
public:
	virtual ~CLowerFlip();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual void					Set_Trigger() override;


	virtual _bool					offsetMatrix(_float4x4* pMatrix)override;
private:
	void			Action_Trigger();

public:
	static unique_ptr<CLowerFlip>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END