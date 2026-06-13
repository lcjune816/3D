#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CButton final : public CTrigger
{
protected:
	CButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CButton(const CButton& Prototype);
public:
	virtual ~CButton();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Late_Interaction(_float fTimeDelta, _bool bOtherTrigger = false);

	virtual void					TriggerToTrigger()override;
	virtual void					Set_Trigger() override;

private:
	void			Action_Trigger(_fvector vPos);
private:
	_float4			m_vOriginPos;

public:
	static unique_ptr<CButton>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END