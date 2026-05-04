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
	typedef struct PlayerState
	{
		_bool bRun{ false }, bCrouch{ false }, bIdle{ false }, bMove{ false }, bJump{ false }, bFalling{ false };
	}PLAYER_STATE;
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
	string					Model_Animation(const vector<string>& pNames);
	void					Connet_Player(shared_ptr<class CPlayer> pPlayer) { m_pPlayer = pPlayer; }
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }

private:
	HRESULT					Ready_Component();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CAnimator>		m_pAnimator;
	weak_ptr<class CPlayer>				m_pPlayer;
	vector<shared_ptr<CVIBuffer>>		m_pMeshList;

private:
	_float4x4							m_bones[BONE_MATRIX];

public:
	static unique_ptr<CPLayer_RightHand> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

