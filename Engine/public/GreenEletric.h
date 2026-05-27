#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGreenElectric final : public CTrigger
{
private:
	CGreenElectric(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CGreenElectric(const CGreenElectric& Prototype);
public:
	virtual ~CGreenElectric();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger= false);
	virtual HRESULT Interaction( _float fTimeDelta,  _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction( _float fTimeDelta, _bool bOtherTrigger = false);

	virtual _bool					offsetMatrix(_float4x4* pMatrix)override;
	virtual void				Set_Trigger() override;
	virtual void				TriggerToTrigger() override;
private:
	void			Action_Trigger();
private:
	_float						m_fRotationArrow = {1.f };

	
public:
	static unique_ptr<CGreenElectric>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>		 Clone(void* pArg) override;

};

NS_END