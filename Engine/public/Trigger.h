#pragma once
#include "Transform.h"
NS_BEGIN(Engine)
enum class TRIGGER_EVENT{BATTERY, DOOR, ELECTRIC, END};
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

	
	uint32_t						Get_TargetNumber() { return m_iTargetNumber; }
	_bool							Set_DstTransform(CTransform* pTransform);
	void							Set_Trigger() { m_bTriggerOn = true; }
	void							Set_TargetNumber(uint32_t iTargetNumber) { m_iTargetNumber = iTargetNumber; }
	void							Set_OtherTrigger(_bool bTrigger) {m_bOtherTrigger = bTrigger;}
	_bool							Get_OtherTrigger() { return m_bOtherTrigger; }
	const TRIGGER_EVENT				Get_Trigger_Event() { return m_eEventTrigger; }
protected:
	_bool							m_bTriggerOn = { false }, m_bOtherTrigger = { false };
	_float							m_fFrameTick{}, m_fFrameTime{};
	_float							m_fAngle{};

	CTransform*						m_pDstTransform = { nullptr };
	uint32_t						m_iTargetNumber = {};
	TRIGGER_EVENT					m_eEventTrigger;

public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;

};

NS_END

