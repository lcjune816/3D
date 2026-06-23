#pragma once
#include "Player.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
}
NS_BEGIN(Client)


class CPLayer_RightHand : public CPlayer
{
public:
	typedef struct strPlayerRightHand : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4* ParentsMatrix; 
		_float4x4   BoneoffsetMatrix;
		
	}RIGHT_HAND_DESC;
protected:
	enum class RIGHTEFFECT { SPARK1 = 2, SPARK2};
protected:
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
	void					Connet_Player(shared_ptr<CGameObject> pPlayer, FSM HAND, shared_ptr<CFSM_Machine>pFsmMachine, shared_ptr<class CFSM_RightHand> pState, int32_t iKey);
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
	shared_ptr<class CWorldLight>		m_pLight{ nullptr };
	vector<shared_ptr<CPLayer_RightHand>> m_pRHandEffect{ };
protected:
	weak_ptr<class CPlayer>				m_pPlayer;
private:
	int32_t								m_iOffsetIndex = {};
	_float4x4							m_fOffsetMatrix, m_FinalWorldMatrix;

	_float4x4*							m_ParentsMatrix{};
	HAND_STATE							m_tagHandState = {};
	PLAYER_HAND							m_eRHand = { PLAYER_HAND::END };
public:
	static unique_ptr<CPLayer_RightHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

