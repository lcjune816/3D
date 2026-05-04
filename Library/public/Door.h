#pragma once
#include "Trigger.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDoor final : public CTrigger
{
protected:
	CDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CDoor(const CDoor& Prototype);
public:
	virtual ~CDoor();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(shared_ptr<class CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger = false) ;

private:
	void			Action_Trigger(shared_ptr<class CTransform> pTransform);
private:
	_float						m_fRotationArrow = {1.f};
public:
	static unique_ptr<CDoor>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END