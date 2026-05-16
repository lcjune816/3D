#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBattery final : public CTrigger
{
protected:
	CBattery(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBattery(const CBattery& Prototype);
public:
	virtual ~CBattery();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(_float fTimeDelta,  _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger = false);
	virtual void					Set_Trigger() override;

private:
	void			Action_Trigger();
private:
	_float						m_fRotationArrow = { 1.f };
	_float						m_fDropTime = {};
public:
	static unique_ptr<CBattery>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END