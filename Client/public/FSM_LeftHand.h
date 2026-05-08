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
	virtual HRESULT Initialize(void* pArg);
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

public:
	void			Set_LeftHand(shared_ptr<CGameObject> pObj);
	void			Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer);
	void			Mouse_Cal();

	void			Hand_End(CPlayer* Player);
	void			Hand_Collision_Check(const PLAYER_HAND eHand);

	void			Set_Bone();
private:
	_bool			 m_bLeftHand{ false }, m_bEndHand{ false }, m_bEndInHand{ false };

	_float			m_fShootTime{ 0 }, m_fShootMaxTime{ 0 }, m_fShootTimeTick{ 0 }, m_fSpeed{ 0 };
	_float3			m_fLastHandPos{}, m_fMouseLook{}, m_fFirstLook{}, m_fStartPos{};;

	int32_t		m_iHandindex{}, m_iFirstHandindex{}, m_iHandAttachedindex{};

	vector<int32_t>		m_ShootBone{};

	shared_ptr<class CPlayer_LeftHand>		m_pHand;
public:
	static unique_ptr<CFSM_LeftHand> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;