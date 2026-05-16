#pragma once
#include "Transform.h"
NS_BEGIN(Engine)
enum class TRIGGER_EVENT{BATTERY, DOOR,PANNEL, GELECTRIC, BELECTRIC, LEVER,ROLLUPDOOR, BATTERYCASE , ELECTRICPOLE, END};
enum class TRIGGER_FLAG
{
	SHADER = 0x0000001, FTRIGGER = 0x0000002, OTHERTRIGGER = 0x0000004, ATTACHED = 0x00000008, CANCLE = 0x00000010 ,END = 0xffffffff
};
class ENGINE_DLL CTrigger abstract : public CComponent
{

public:
typedef struct tagTriggerdesc
{
	_bool bTrigger;
 }TRIGGER_DESC;

public:

protected:
	typedef struct strBindResource
	{
		_float4 fColor{1,1,1,1};
	}BIND_RESOURCE;
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
	_bool							Set_DstTransform(shared_ptr<CTransform> pTransform);
	
	void							Set_TargetNumber(uint32_t iTargetNumber) { m_iTargetNumber = iTargetNumber; }
	void							Set_OtherTrigger(_bool bTrigger) {m_bOtherTrigger = bTrigger;}
	_bool							Get_OtherTrigger() { return m_bOtherTrigger; }
	uint32_t						Get_FlagState(uint32_t iFlag) { return Check_Flag(iFlag); }
	const TRIGGER_EVENT				Get_Trigger_Event() { return m_eEventTrigger; }
	void							Set_Flag(uint32_t iFlag, FLAGVALUE eValue);
	void							Set_PtrMatrix(_float4x4* pMat) { m_pMatrixPtr = pMat; }
	void							Disconnect_Transform() { m_pDstTransform.reset(); }

	virtual void					Set_Trigger() PURE;

	_bool							Check_Flag(uint32_t iFlag);
	virtual	void					Bind_Resource(shared_ptr<class CShader> pShader, const _char* pConstantName);
	virtual _bool					offsetMatrix(_float4x4* pMatrix);
protected:
	_bool							m_bTriggerOn = { false }, m_bOtherTrigger = { false };
	_float							m_fFrameTick{}, m_fFrameTime{};
	_float							m_fAngle{};
	_float4x4*						m_pMatrixPtr = { nullptr };

	weak_ptr<CTransform>			m_pDstTransform;
	uint32_t						m_iTargetNumber = {};
	uint32_t						m_iFlag = { 0 };
	TRIGGER_EVENT					m_eEventTrigger;
	BIND_RESOURCE					m_BindValue;

	weak_ptr<class CGameObject>		m_pParent;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;

};

NS_END

