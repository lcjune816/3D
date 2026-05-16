#pragma once
#include "Player.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
}
NS_BEGIN(Client)

class CPlayer_LeftHand final : public CPlayer
{
public:
	typedef struct strPlayerLeftHand
	{
		_float4x4* ParentsMatrix;
		_float4x4   BoneoffsetMatrix;
	}LEFT_HAND_DESC;
private:
	CPlayer_LeftHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer_LeftHand(const CPlayer_LeftHand& Prototye);
public:
	virtual ~CPlayer_LeftHand();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();


public:
	void					Connet_Player(shared_ptr<CGameObject> pPlayer, FSM HAND, shared_ptr<CFSM_Machine>pFsmMachine, shared_ptr<class CFSM_LeftHand> pState, int32_t iKey);
	const PLAYER_HAND		Get_PlayerHand() { return m_eRHand; }
	HAND_STATE&				Get_HandState() { return m_tagHandState; }
	shared_ptr<CGameObject>	Get_Arm();

	void					Hand_Pivot();

private:
	void					Bind_ResourceFromFlag(CShader* pShader, const _char* pConstantName);
	void					State_Move();
	HRESULT					Ready_Component();
private:
	shared_ptr<class CPlayer_Arm>		m_pArm = { nullptr };
	weak_ptr<class CPlayer>				m_pPlayer;

private:
	int32_t								m_iOffsetIndex = {};
	_float4x4							 m_fOffsetMatrix, m_FinalWorldMatrix;
	_float4x4							m_bones[BONE_MATRIX];

	_float4x4* m_ParentsMatrix{};
	HAND_STATE							m_tagHandState = {};
	PLAYER_HAND							m_eRHand = { PLAYER_HAND::END };
public:
	static unique_ptr<CPlayer_LeftHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

