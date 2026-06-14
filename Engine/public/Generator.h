#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGenerator final : public CTrigger , public CObserver
{
protected:
	CGenerator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CGenerator(const CGenerator& Prototype);
public:
	virtual ~CGenerator();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false);

	virtual void					TriggerToTrigger()override;
	virtual void					Set_Trigger() override;

	virtual void					 OnNotify(const EVENT& event) override;
private:
	void			Action_Work(const _float& fTimeDelta);

private:
	void			Action_Trigger();
private:
	_bool			m_bOne{ false };

public:
	static unique_ptr<CGenerator>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END