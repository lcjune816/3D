#pragma once
#include "Player_FSM.h"

NS_BEGIN(Client)
class  CFSM_RightHand : public CPlayer_FSM
{

private:
	CFSM_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_RightHand(const CFSM_RightHand& Prototype);
public:
	virtual ~CFSM_RightHand();

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

	void			Set_RightHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm);
	void			Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer, const _float& fTimeDelta,  _bool bFinished = false);
	void			Mouse_Cal();
	
	void			Hand_End(CPlayer* Player);
	void			Hand_Collision_Check(const PLAYER_HAND eHand);

private:
	_bool			m_bRightHand{ false }, m_bEndHand{ false }, m_bEndInHand{ false }, m_bCollision{ false };

	_float			m_fShootTime{ 0 }, m_fShootMaxTime{ 0 }, m_fShootTimeTick{ 0 }, m_fSpeed{ 0 };
	_float3			m_fLastHandPos{}, m_fMouseLook{}, m_fFirstLook{}, m_fStartPos{}, m_fOffset{};

	int32_t		m_iHandindex{}, m_iFirstHandindex{}, m_iHandAttachedindex{};

	vector<int32_t>		m_ShootBone{};
	vector<_float3>		m_EdgePoses;
	vector<_float3>		m_EdgeNormals;
	_float4x4* m_StartMatrix = { nullptr };
	shared_ptr<class CPlayer_Arm>			m_pArm;
	shared_ptr<class CPLayer_RightHand>		m_pHand;
public:
	static unique_ptr<CFSM_RightHand> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;