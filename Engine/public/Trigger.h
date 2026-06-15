#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
enum class TRIGGER_EVENT{BATTERY, DOOR,PANNEL, GELECTRIC, BELECTRIC, LEVER,ROLLUPDOOR, BATTERYCASE , ELECTRICPOLE, PUZZLEROT,GENERATOR, ELEVATOR, BUTTON,
						POLEHEAD,END};
enum class TRIGGER_FLAG
{
	SHADER = 0x0000001, FTRIGGER = 0x0000002, OTHERTRIGGER = 0x0000004, ATTACHED = 0x00000008, CANCLE = 0x00000010 ,PAUSE = 0x00000020, WORLD_EVENT = 0x00000040, END = 0xffffffff
};

class ENGINE_DLL CTrigger abstract : public CComponent
{

public:
typedef struct tagTriggerdesc : public CGameObject::GAMEOBJECT_DESC
{
	_bool bTrigger;
	TRIGGER_ROT eRot{TRIGGER_ROT::X};
	WORLD_EVENT eWroldEvent{ WORLD_EVENT::END };
	_float fMaxFrameTime{0.f};
	_float fFrameTickTime{0.f};
	_float fRotationArrow{0.f};
	_float fArrrowRotation{0.f};
	_float3 vLook = {};
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
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	virtual HRESULT					Pirority_Interaction( _float fTimeDelta, _bool bOtherTrigger = false);
	virtual HRESULT					Interaction(_float fTimeDelta, _bool bOtherTrigger = false) PURE;
	virtual HRESULT					Late_Interaction(_float fTimeDelta,  _bool bOtherTrigger = false)PURE;

	void							Set_Parent(shared_ptr<class CGameObject> pObj);
	uint32_t						Get_TargetNumber() { return m_iTargetNumber; }
	_bool							Set_DstTransform(shared_ptr<class CTransform> pTransform);
	
	_bool							StartPos(_fvector StartPos, _fvector OffsetPos, _float4* OutPos, const _float fTimeDelta);
	_bool							EndPos(_fvector StartPos, _fvector OffsetPos,   _float4* OutPos, const _float fTimeDelta);


	void							Set_TargetNumber(uint32_t iTargetNumber) { m_iTargetNumber = iTargetNumber; }
	void							Set_OtherTrigger(_bool bTrigger) {m_bOtherTrigger = bTrigger;}
	_bool							Get_OtherTrigger() { return m_bOtherTrigger; }
	uint32_t						Get_FlagState(uint32_t iFlag) { return Check_Flag(iFlag); }


	_bool							Check_Trigger_Event(TRIGGER_EVENT eEvent) { if (eEvent == m_eEventTrigger) return true;   return false; }
	const TRIGGER_EVENT				Get_Event() { return m_eEventTrigger; }
	void							Set_Flag(uint32_t iFlag, FLAGVALUE eValue);
	void							Set_PtrMatrix(_float4x4* pMat) { m_pMatrixPtr = pMat; }
	void							Disconnect_Transform() { m_pDstTransform.reset(); }

	virtual void					Set_Trigger() PURE;

	_bool							Check_Flag(uint32_t iFlag);
	virtual void					TriggerToTrigger();
	virtual	void					Bind_Resource(shared_ptr<class CShader> pShader, const _char* pConstantName);
	virtual _bool					offsetMatrix(_float4x4* pMatrix);
protected:
	_bool							Start_Rotation(const _float& fTimeDelta);
	_bool							End_Rotation(const _float& fTimeDelta);
	_bool							Timer_Flag(uint32_t flag, FLAGVALUE eValue , const _float& fTimeDelta);
	
protected:
	_bool							m_bTriggerOn = { false }, m_bOtherTrigger = { false };
	_float							m_fFrameTick{0.f}, m_fFrameTickTime{ 0.f }, m_fFrameTime{ 0.f }, m_fMaxFrameTime{ 0.f };
	_float							m_fAngle{}, m_fStartAngle{ 0 }, m_fEndAngle{ 180.f }, m_fRotationArrow = { 1.f }, m_fFlagCnt{0 };
	_float4x4*						m_pMatrixPtr = { nullptr };

	weak_ptr<class CTransform>		m_pDstTransform;
	uint32_t						m_iTargetNumber = {};
	uint32_t						m_iFlag = { 0 };
	TRIGGER_EVENT					m_eEventTrigger;
	BIND_RESOURCE					m_BindValue;
	TRIGGER_STATE					m_eState{ TRIGGER_STATE::IDLE };
	_float4							m_fRotation{};
	weak_ptr<class CGameObject>		m_pParent;
	uint32_t						m_iLevel;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;

};

NS_END

