#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
namespace Engine
{
	class CAnimator;
}
NS_BEGIN(Client)


class CPlayer_LeftHand final : public CGameObject
{
private:
	typedef struct HandState
	{
		_bool bHandAttached{ false }, bShoot{ false };
	}HAND_STATE;
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
	string					Model_Animation(const vector<string>& pNames);
	void					Connet_Player(shared_ptr<class CPlayer> pPlayer, int32_t iOffsetIndex) { m_pPlayer = pPlayer; m_iOffsetIndex = iOffsetIndex; }
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }

	void					Enable_Electric();
	const PLAYER_HAND		Get_PlayerHand() { return m_eLHand; }
	const _float4x4         Get_FirstMatrix() { return m_StartMatrix; }
	const _float4x4			Get_LastMatrix() { return m_LastMatrix; }
	HAND_STATE&			    Get_HandState() { return m_tagHandState; }
private:
	void					Hand_Pivot();
	void					Hand_Collision();
	void					Hand_Trigger_Event(class CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger);
private:
	HRESULT					Ready_Component();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CAnimator>		m_pAnimator;
	weak_ptr<class CPlayer>				m_pPlayer;
	vector<shared_ptr<CVIBuffer>>		m_pMeshList;

private:
	int32_t								m_iOffsetIndex = {};

	_float4x4							m_LastMatrix;
	_float4x4							m_StartMatrix;
	_float4x4							m_bones[BONE_MATRIX];

	PLAYER_HAND							m_eLHand = { PLAYER_HAND::END };

	HAND_STATE							m_tagHandState = {};
public:
	static unique_ptr<CPlayer_LeftHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

