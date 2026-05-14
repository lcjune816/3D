#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
}
NS_BEGIN(Client)

typedef struct HandState
{
	_bool bHandAttached{ false }, bShoot{ false }, EndForce{ false }, bElectric{ false }, bCollect{ false };
}HAND_STATE;

class CPLayer_RightHand final : public CGameObject
{
public:
	typedef struct strPlayerRightHand
	{
		_float4x4* ParentsMatrix; 
		_float4x4   BoneoffsetMatrix;
	}RIGHT_HAND_DESC;
private:
	CPLayer_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPLayer_RightHand(const CPLayer_RightHand& Prototye);
public:
	virtual ~CPLayer_RightHand();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();

public:
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }
	void					Connet_Player(shared_ptr<CGameObject> pPlayer, FSM HAND, shared_ptr<CFSM_Machine>pFsmMachine, shared_ptr<class CFSM_RightHand> pState, int32_t iKey);
	void					State_Move();
	const PLAYER_HAND		Get_PlayerHand() { return m_eRHand; }
	HAND_STATE&				Get_HandState() { return m_tagHandState; }
	const _float4x4			Get_LastMatrix() { return m_LastMatrix; }
	shared_ptr<CGameObject>	Get_Arm();
private:
	void					Hand_Pivot();
private:
	HRESULT					Ready_Component();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CAnimator>		m_pAnimator = { nullptr };
	shared_ptr<class CPlayer_Arm>		m_pArm = { nullptr };
	weak_ptr<class CPlayer>				m_pPlayer;
	vector<shared_ptr<CVIBuffer>>		m_pMeshList;
	shared_ptr<Engine::CFSM_Machine>    m_pStateMachine = { nullptr };

private:
	int32_t								m_iOffsetIndex = {};
	_float4x4							m_LastMatrix;

	_float4x4*							m_ParentsMatrix{};
	_float4x4							m_bones[BONE_MATRIX];

	_float4x4							m_fOffsetMatrix;

	_float4x4							m_FinalWorldMatrix;
	PLAYER_HAND							m_eRHand = { PLAYER_HAND::END };

	HAND_STATE							m_tagHandState = {};
public:
	static unique_ptr<CPLayer_RightHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

