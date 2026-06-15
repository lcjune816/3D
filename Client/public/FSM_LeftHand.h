#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_LeftHand : public CPlayer_FSM
{
	enum class ARROW { LEFT, RIGHT, END };

private:
	CFSM_LeftHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_LeftHand(const CFSM_LeftHand& Prototype);
public:
	virtual ~CFSM_LeftHand();

public:
	virtual HRESULT		 Initialize(void* pArg);
	virtual void		 Enter_State() override;
	virtual void		 Update_State(_float fTimeDelta)override;
	virtual void		 Exit_State()  override;
	void				 Set_LeftHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm);

private:
	void				Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer, const _float& fTimeDelta, class CPlayer_Arm* PlayerArm, class CPlayer_LeftHand* pRHand, _bool bFinished = false);

	void				Hand_End(CPlayer* Player);
	void				Hand_Collision_Check(shared_ptr<CPlayer_LeftHand> pObj, shared_ptr<CPlayer_Arm> pArm, const _float& fTimeDelta);

	_bool				Hand_Trigger_Event(shared_ptr<CPlayer_LeftHand> pObj, class CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger, CTransform* pTransform, const _float& fTimeDelta);
	virtual void		Hand_State_Chand(CHANGE_STATE eChange) override;

private:
	_float3			m_fLastHandPos{}, m_fMouseLook{}, m_fFirstLook{}, m_fStartPos{}, m_fOffset{}, m_fForce{};


	vector<GRAB_ARM_EDGE>						m_EdgePoses;

	_bool								m_bSound{ false };
	_float4x4* m_StartMatrix = { nullptr };
	uint32_t									m_iEdgeCnt = {}, m_iMaxSpeed{ 400 };
	vector<uint32_t>							m_iSizeCnt;
	weak_ptr<class CPlayer_Arm>					m_pArm;
	weak_ptr<class CPlayer_LeftHand>			m_pHand;
public:
	static unique_ptr<CFSM_LeftHand> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;