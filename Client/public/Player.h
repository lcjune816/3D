#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "GameInstance.h"
namespace Engine
{
	class CAnimator;
	class CFSM_Machine;
	class CNavigation;
}
NS_BEGIN(Client)
//Couch enter 앉을때 애니메이션 couch pose 앉은 후 애니메이션 couch exit 일어날대 애니메이션
//jump 점프 falling 떨어질떄 land 착지있음 falshlight shoot flae 그 빵야 
//grabpack shoot in 다시 들어왔을떄 out 나갈떄 두개로 분리됨
// 
enum class PLAYER_ANIME {CROUCH_ENTER, CROUCH_EXIT, CROUCH_POSE, FALLING, SHOOT_FLARE,	SHOOT_IN, SHOOT_OUT, HAND_SWITCH, IDLE, JUMP, LAND, LOWER, DIR_BACKWARD, DIR_FOWARD, DIR_LEFT,DIR_RIGHT,CATDEATH2, RAISE,RUN, WALK,WALK_STOP,
						SHOOTIN_L,SHOOTIN_R, SHOOTOUT_L, SHOOTOUT_R,END};
enum class PLAYER_MACHINE{NORMAL, LEFT_HAND, RIGHT_HAND,END};

enum class PLAYER_FLAG { ELECTRIC_SHORT = 0x00000001, ELECTRIC_LONG = 0x00000002, TIMER = 0x00000004,CONNECTHAND = 0x00000008,
				RUN = 0x00000010, CROUCH = 0x00000020, IDLE = 0x00000040, MOVE = 0x00000080, JUMP = 0x00000100, FALLING = 0x00000200,	END = 0xffffffff };
class CPlayer : public CGameObject
{
protected:
	typedef struct HandState
	{
		_bool bHandAttached{ false }, bShoot{ false }, EndForce{ false }, bElectric{ false }, bCollect{ false };
	}HAND_STATE;
private:
	typedef struct PlayerState
	{
		_bool bRun{ false }, bCrouch{ false }, bIdle{ false }, bMove{ false }, bJump{ false }, bFalling{ false }, bLHand{ false }, bTwoHand{ false }, bRHand{};
	}PLAYER_STATE;
protected:
	CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer(const CPlayer& Prototye);
public:
	virtual ~CPlayer();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();

public:
	string					Model_Animation(const vector<string>& pNames);
	const MOVE&				Get_State() { return m_eState; }
	PLAYER_ANIME			Get_Animation_State() { return m_eAnimeState; }
	PLAYER_STATE&			Get_AnimeState() { return m_ePlayer; }   //이걸로 fsm에서 bool값 조정하기

	void					Set_ActionState(_bool	bAction) { m_bOnlyActionState = bAction; }
	void					Change_Animation(PLAYER_ANIME eAnime, _bool bLoop = true, _bool bForce = false, _bool Blend = true);
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }
	CAnimator*				GetAnimator() { return m_pAnimator.get(); }
	void					Set_Flag(uint32_t eState, FLAGVALUE eValue);
	_bool					Flag_Check(uint32_t iFlag);

protected:
	void					Timer(const _float& fTimeDelta);

private:
	void					Turn(const _float& fTimeDelta);
	HRESULT					Ready_Component(void* pArg);
	void					Default_Height();
	void					State_Move();
	void					Hnad_State_Check();
protected:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CFSM_Machine>	m_pStateMachine;
	shared_ptr<Engine::CAnimator>		m_pAnimator;

	vector<shared_ptr<CVIBuffer>>		m_pMeshList;


	uint32_t							m_iStateFlag{ 0 };
	_float2								m_iMousePos{ 0 ,0}, m_iCurrentPos{0,0}, m_fTimerAim{ 0,0 }, m_fTimerAimCnt{ 0 ,0};
private:
	shared_ptr<class CPlayer_LeftHand>	m_pPlayerLHand = { nullptr };
	shared_ptr<class CPLayer_RightHand>	m_pPlayerRHand = { nullptr };
	shared_ptr<class CNavigation>		m_pNavigation = { nullptr };
	shared_ptr<class CPlayerAim>		m_pAim = { nullptr };
	vector<string>						m_ShootBone;
private:
	_float								m_fTimerTick{ 0 }, m_fTimerCnt{ 0 };
	_float4x4							m_bones[BONE_MATRIX];

	PLAYER_ANIME						m_eAnimeState = {};
	PLAYER_STATE						m_ePlayer = {}; //플레이어 상태 조정 bool값 모음ㄱ
	PLAYER_FLAG							m_eFlag = {};
	MOVE								m_eState = { MOVE::END };
private:
	_bool								m_bOnlyActionState = { false }, m_bDelay[2] = {};
public:
	static unique_ptr<CPlayer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

