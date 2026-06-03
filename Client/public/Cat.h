#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"
namespace Engine
{
	class CAnimator;
	class CFSM_Machine;
}
NS_BEGIN(Client)

enum class CAT_ANIME {
	DEAD_SLOW, DEAD_FAST, OVERSHOOTWALK, OVERWAL, DOORKICK, JUMPSCALE, SMASH, WALK, END
};

class CBoss_Cat final : public CGameObject
{
private:
	CBoss_Cat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBoss_Cat(const CBoss_Cat& Prototye);
public:
	virtual ~CBoss_Cat();

	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render();

public:
	string					Model_Animation(const vector<string>& pNames);

	const MOVE&				 Get_State() { return m_eState; }
	CAT_ANIME				Get_Animation_State() { return m_eAnimeState; }
	_float4x4*				 Get_OtherMatrixPtr() { return m_pOtherMatrix; }
	void					Set_ActionState(_bool	bAction) { m_bOnlyActionState = bAction; }
	void					Change_Animation(CAT_ANIME eAnime, _bool bLoop = true, _bool bForce = false);
	_bool					Animation_End() { return m_pAnimator->Animation_End(); }
private:
	HRESULT					Ready_Component();
	void					State_Move();
private:

	shared_ptr<Engine::CShader>			m_pShaderCom = { nullptr };
	shared_ptr<Engine::CFSM_Machine>	m_pStateMachine;
	shared_ptr<Engine::CAnimator>		m_pAnimator;
	shared_ptr<class CCat_Fog>				m_pFogEffect;
	vector<shared_ptr<CVIBuffer>>		m_pMeshList;

	shared_ptr<class CNavigation>		m_pNavigation;

private:

	_float4x4* m_pOtherMatrix = {};
	_float4x4							m_bones[BONE_MATRIX];

	CAT_ANIME							m_eAnimeState = {};

	_bool								m_bOnlyActionState = { false };
	MOVE								m_eState = { MOVE::END };
public:
	static unique_ptr<CBoss_Cat> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg) override;
};

NS_END

