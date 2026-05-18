#pragma once
#include "Player_FSM.h"
#include "Player_RightHand.h"
NS_BEGIN(Client)
class  CFSM_RightHand : public CPlayer_FSM
{
private:
	CFSM_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_RightHand(const CFSM_RightHand& Prototype);
public:
	virtual ~CFSM_RightHand();

public:
	virtual	 HRESULT Initialize(void* pArg);
	virtual	 void	 Enter_State() override;
	virtual	 void	 Update_State(_float fTimeDelta)override;
	virtual	 void	 Exit_State()  override;
	void			 Set_RightHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm);

private:
	void			 Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer, const _float& fTimeDelta, class CPlayer_Arm* PlayerArm, class CPLayer_RightHand* pRHand, _bool bFinished = false);
					 
	void			 Hand_End(CPlayer* Player);
	void			 Hand_Collision_Check(shared_ptr<CPLayer_RightHand> pObj, shared_ptr<CPlayer_Arm> pArm ,const _float& fTimeDelta);
	void			 Update_LastPos(class CTriggerObject* pTrigger, class CTransform* pTransform);
	_bool			 Hand_Trigger_Event(shared_ptr<CPLayer_RightHand> pObj,class CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger,CTransform* pTransform,const _float& fTimeDelta);
	virtual void	 Hand_State_Chand(CHANGE_STATE eChange)override;
private:

	_float3			m_fLastHandPos{}, m_fMouseLook{}, m_fFirstLook{}, m_fStartPos{}, m_fOffset{}, m_fForce{};
	_bool			m_bTimeStop{ false };

	vector<GRAB_ARM_EDGE>						m_EdgePoses;

	_float4x4*									m_StartMatrix = { nullptr };
	uint32_t									m_iEdgeCnt = {};
	_float										m_fLength = {}, Velocity = {};
	vector<uint32_t>							m_iSizeCnt;
	_bool										m_bOnlyone = { false };
	
	weak_ptr<class CPlayer_Arm>					m_pArm;
	weak_ptr<class CPLayer_RightHand>			m_pHand;
public:
	static unique_ptr<CFSM_RightHand> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;