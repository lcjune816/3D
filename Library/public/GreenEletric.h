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

	virtual HRESULT Interaction(shared_ptr<class CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger = false);

private:
	void			Action_Trigger();
private:
	_float						m_fRotationArrow = {1.f };

	
public:
	static unique_ptr<CGreenElectric>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>		 Clone(void* pArg) override;

};

NS_END