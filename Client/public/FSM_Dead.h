#pragma once
#include "Player_FSM.h"
#include "Observer.h"
NS_BEGIN(Client)
class  CFSM_Dead final : public CPlayer_FSM, public CObserver 
{
private:
	CFSM_Dead(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Dead(const CFSM_Dead& Prototype);
public:
	virtual ~CFSM_Dead();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

	virtual void OnNotify(const EVENT& eEvent)override;

private:
	void	OffsetTime(const _float& fTimeDelta);

	void    ShakingCam1(shared_ptr<CTransform> pTransform, const _float& fTimeDelta);
	void    ShakingCam2(shared_ptr<CTransform> pTransform, const _float& fTimeDelta);
	void    ShakingCam3(shared_ptr<CTransform> pTransform, const _float& fTimeDelta);

	void	GoingHead(shared_ptr<CTransform> pTransform, const _float& fTimeDelta);
private:
	_float			m_fFrameTick{}, m_fAngle{ 5.f }, m_fTimeOffset{}, m_fAngleOffset{}, m_fFrameTickDead{};
	_float3			m_vTargetLook{};
	_float3			m_vOriginLook{};
	_float3			m_fRotation{};
	_bool			m_bShaking{ false };
	
public:
	static unique_ptr<CFSM_Dead> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;