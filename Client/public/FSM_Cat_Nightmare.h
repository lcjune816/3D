#pragma once
#include "Cat_FSM.h"

NS_BEGIN(Client)
class  CFSM_Cat_Nightmare : public CCat_FSM, public CObserver
{
	typedef struct tagTeacherSpawndesc
	{
		uint32_t		iNaviEventIndex;
	}TEACHER_SPAWN_DESC;

private:
	CFSM_Cat_Nightmare(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Cat_Nightmare(const CFSM_Cat_Nightmare& Prototype);
public:
	virtual ~CFSM_Cat_Nightmare();

public:

	virtual HRESULT Initialize(void* pArg)override;
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;
	virtual void OnNotify(const EVENT& eEvent) override;

private:
	void			Action(shared_ptr<CTransform> pTransform, shared_ptr<CBoss_Cat> pBoss, const _float& fTimeDelta);
	void			Action_Return(shared_ptr<CTransform> pTransform, shared_ptr<CBoss_Cat> pBoss, const _float& fTimeDelta);

	void			Kill_Player(shared_ptr<CTransform> pTransform, shared_ptr<CBoss_Cat> pBoss, const _float& fTimeDelta);
	void			Deaddddd();
private:
	uint32_t		m_iNaviEventIndex{};
	_float3			m_fDestPos;
	_float3			m_fCurrentPos, m_fCurretLook, m_fPreLook;
	_float			m_fAngle{};

	_bool			m_bKill{ false };

	const _tchar* m_SoundName[2];
public:
	static unique_ptr<CFSM_Cat_Nightmare> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;

