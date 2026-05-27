#pragma once
#include "Trigger.h"

#include  "Observer.h"
NS_BEGIN(Engine)

class ENGINE_DLL CRollupDoor final : public CTrigger, public  CObserver
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

	virtual void					 OnNotify(const EVENT& event) override;
private:
	void							Down_Door(const _float& fTimeDelta);
	void							Down_DeadDoor(const _float& fTimeDelta);
	void							Action_Trigger(_float fTimeDelta);
private:
	_float						m_fRotationArrow = { 1.f };
	_float						m_fWorldTime{};
	_float						m_fSrcLength{}, m_fDestLength{}, m_fCurHeight{0};
	_float3						m_fDestPos;
public:
	static unique_ptr<CRollupDoor>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;

};

NS_END