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

	virtual HRESULT Interaction(shared_ptr<class CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger = false);

private:
	void			Action_Trigger(shared_ptr<class CTransform> pTransform, _float fTimeDelta);
private:
	_float						m_fRotationArrow = { 1.f };
public:
	static unique_ptr<CRollupDoor>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END