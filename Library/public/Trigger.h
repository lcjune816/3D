#pragma once
#include "Transform.h"
NS_BEGIN(Engine)
enum class TRIGGER_EVENT{BATTERY, DOOR, ELECTRIC, LEVER,ROLLUPDOOR, BATTERYCASE , END};
class ENGINE_DLL CTrigger abstract : public CComponent
{

public:
typedef struct tagTriggerdesc
{
	_bool bTrigger;
 }TRIGGER_DESC;

public:
	enum class TRIGGER_FLAG
	{
		SHADER = 0x00000001, FTRIGGER = 0x0000002, OTHERTRIGGER = 0x0000004, END = 0xffffffff
	};
	
protected:
	CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTrigger(const CTrigger& Prototype);
public:
	virtual ~CTrigger();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Pirority_Interaction( _float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT Interaction(_float fTimeDelta, _bool bOtherTrigger = false) PURE;
	virtual HRESULT Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger = false)PURE;
	
	void							Set_Parent(shared_ptr<class CGameObject> pObj);
	uint32_t						Get_TargetNumber() { return m_iTargetNumber; }
	_bool							Set_DstTransform(CTransform* pTransform);
	void							Set_Trigger() { m_bTriggerOn = true; }
	void							Set_TargetNumber(uint32_t iTargetNumber) { m_iTargetNumber = iTargetNumber; }
	void							Set_OtherTrigger(_bool bTrigger) {m_bOtherTrigger = bTrigger;}
	_bool							Get_OtherTrigger() { return m_bOtherTrigger; }
	uint32_t						Get_FlagState() { return m_iFlag; }
	const TRIGGER_EVENT				Get_Trigger_Event() { return m_eEventTrigger; }
	void							Set_Flag(TRIGGER_FLAG eFlag, FLAGVALUE eValue);
	void							Disconnect_Transform() { m_pDstTransform = nullptr; }

	virtual _bool					offsetMatrix(_float4x4* pMatrix);
protected:
	_bool							m_bTriggerOn = { false }, m_bOtherTrigger = { false };
	_float							m_fFrameTick{}, m_fFrameTime{};
	_float							m_fAngle{};

	CTransform*						m_pDstTransform = { nullptr };
	uint32_t						m_iTargetNumber = {};
	TRIGGER_EVENT					m_eEventTrigger;

	uint32_t						m_iFlag;

	weak_ptr<class CGameObject>		m_pParent;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;

};

NS_END

