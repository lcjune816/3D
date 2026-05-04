#pragma once
#include "Transform.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTrigger abstract : public CComponent
{
public:
typedef struct tagTriggerdesc
{
	_bool bTrigger;
 }TRIGGER_DESC;
protected:
	CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTrigger(const CTrigger& Prototype);
public:
	virtual ~CTrigger();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Interaction(shared_ptr<class CTransform> pTransform, _float fTimeDelta, _bool bOtherTrigger = false) PURE;

	
	uint32_t			Get_TargetNumber() { return m_iTargetNumber; }

	void			Set_Trigger() { m_bTriggerOn = true; }
	void			Set_TargetNumber(uint32_t iTargetNumber) { m_iTargetNumber = iTargetNumber; }
	void			Set_OtherTrigger(_bool bTrigger) {m_bOtherTrigger = bTrigger;}
protected:
	_bool							m_bTriggerOn = { false }, m_bOtherTrigger = { false };
	_float							m_fFrameTick{}, m_fFrameTime{};
	_float							m_fAngle{};

	uint32_t						m_iTargetNumber = {};
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;

};

NS_END

