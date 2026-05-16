#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBatteryCase final : public CTrigger
{
protected:
	CBatteryCase(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBatteryCase(const CBatteryCase& Prototype);
public:
	virtual ~CBatteryCase();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta,_bool bOtherTrigger = false);
	virtual void					Set_Trigger() override;

public:
	HRESULT			Action_Trigger(weak_ptr<class CTransform> pTransform);
private:
	_float						m_fRotationArrow = { 1.f };
	weak_ptr<class CTransform>		m_pTransform = {};
public:
	static unique_ptr<CBatteryCase>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END