#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
	class CFSM_Machine;
}
NS_BEGIN(Client)
//Couch enter 앉을때 애니메이션 couch pose 앉은 후 애니메이션 couch exit 일어날대 애니메이션
//jump 점프 falling 떨어질떄 land 착지있음 falshlight shoot flae 그 빵야 
//grabpack shoot in 다시 들어왔을떄 out 나갈떄 두개로 분리됨
// 
enum class TEACHER_ANIME {
	DEAD_SLOW,DEAD_FAST, OVERSHOOTWALK, OVERWAL, DOORKICK ,JUMPSCALE,SMASH, WALK,END
};

class CBoss_Teacher final : public CGameObject
{
private:
	typedef struct strtagTeacherstate
	{
		_bool bRun{ false }, bCrouch{ false }, bIdle{ false }, bMove{ false }, bJump{ false }, bFalling{ false }, bHand{ false }, bLHand{ false }, bTwoHand{ false }, bRHand{};
	}TEACHER_STATE;
private:
	CBoss_Teacher(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBoss_Teacher(const CBoss_Teacher& Prototye);
public:
	virtual ~CBoss_Teacher();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();

public:
	string					Model_Animation(const vector<string>& pNames);

	const MOVE& Get_State() { return m_eState; }
	TEACHER_ANIME			Get_Animation_State() { return m_eAnimeState; }
	TEACHER_STATE&			Get_AnimeState() { return m_eBoss; }   //이걸로 fsm에서 bool값 조정하기
	_float4x4*				 Get_OtherMatrixPtr() { return m_pOtherMatrix; }
	void					Set_ActionState(_bool	bAction) { m_bOnlyActionState = bAction; }
	void					Change_Animation(TEACHER_ANIME eAnime, _bool bLoop = true, _bool bForce =false);
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }
	CAnimator*				GetAnimator() { return m_pAnimator.get(); }
private:
	HRESULT					Ready_Component();
	void					State_Move();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CFSM_Machine>	m_pStateMachine;
	shared_ptr<Engine::CAnimator>		m_pAnimator;

	vector<shared_ptr<CVIBuffer>>		m_pMeshList;

	shared_ptr<class CNavigation>		m_pNavigation;

private:
	vector<string>						m_ShootBone;

	_float4x4*							m_pOtherMatrix = {};
	_float4x4							m_bones[BONE_MATRIX];

	TEACHER_ANIME						m_eAnimeState = {};
	TEACHER_STATE						m_eBoss = {}; 

	_bool								m_bOnlyActionState = { false };
	MOVE								m_eState = { MOVE::END };
public:
	static unique_ptr<CBoss_Teacher> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

