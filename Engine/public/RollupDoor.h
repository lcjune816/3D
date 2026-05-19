#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRollupDoor final : public CTrigger
{
protected:
	CRollupDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CRollupDoor(const CRollupDoor& Prototype);
public:
	virtual ~CRollupDoor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Pirority_Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false)override;
	virtual HRESULT Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger = false)override;

	virtual void					Set_Trigger() override;
	virtual void					TriggerToTrigger()override;
private:
	void			Action_Trigger(_float fTimeDelta);
private:
	_float						m_fRotationArrow = { 1.f };
public:
	static unique_ptr<CRollupDoor>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END