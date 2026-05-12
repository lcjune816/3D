#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
}
NS_BEGIN(Client)



class CPLayer_RightHand final : public CGameObject
{
private:
	typedef struct HandState
	{
		_bool bHandAttached{ false }, bShoot{ false }, EndForce{ false }, bElectric{ false };
	}HAND_STATE;
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
	void					Connet_Player(shared_ptr<class CPlayer> pPlayer, int32_t iOffsetIndex) { m_pPlayer = pPlayer; m_iOffsetIndex = iOffsetIndex; }

	const PLAYER_HAND		Get_PlayerHand() { return m_eRHand; }
	HAND_STATE&				Get_HandState() { return m_tagHandState; }
	const _float4x4         Get_FirstMatrix() {return m_StartMatrix;}
	const _float4x4			Get_LastMatrix() { return m_LastMatrix; }
	shared_ptr<CGameObject>	Get_Arm();
private:
	void					Hand_Pivot();
	void					Hand_Collision();
	void					Hand_Trigger_Event(class CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger);
private:
	HRESULT					Ready_Component();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CAnimator>		m_pAnimator = { nullptr };
	shared_ptr<class CPlayer_Arm>		m_pArm = { nullptr };
	weak_ptr<class CPlayer>				m_pPlayer;
	vector<shared_ptr<CVIBuffer>>		m_pMeshList;


private:
	int32_t								m_iOffsetIndex = {};

	_float4x4							m_LastMatrix;
	_float4x4							m_StartMatrix;
	_float4x4							m_bones[BONE_MATRIX];


	PLAYER_HAND							m_eRHand = { PLAYER_HAND::END };

	HAND_STATE							m_tagHandState = {};
public:
	static unique_ptr<CPLayer_RightHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

