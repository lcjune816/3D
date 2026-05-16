#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CElectricPannel final : public CTrigger
{
private:
	CElectricPannel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CElectricPannel(const CElectricPannel& Prototype);
public:
	virtual ~CElectricPannel();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;

	virtual void					Set_Trigger() override;

	virtual _bool					offsetMatrix(_float4x4* pMatrix)override;
private:
	void							Action_Trigger();
private:
	_float						m_fRotationArrow = { 1.f };


public:
	static unique_ptr<CElectricPannel>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>		 Clone(void* pArg) override;

};

NS_END