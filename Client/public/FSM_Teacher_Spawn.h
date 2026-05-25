#pragma once
#include "Teacher_FSM.h"

NS_BEGIN(Client)
class  CFSM_Teacher_Spawn : public CTeacher_FSM , public CObserver
{
	typedef struct tagTeacherSpawndesc
	{
		uint32_t		iNaviEventIndex;
	}TEACHER_SPAWN_DESC;

private:
	CFSM_Teacher_Spawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CFSM_Teacher_Spawn(const CFSM_Teacher_Spawn& Prototype);
public:
	virtual ~CFSM_Teacher_Spawn();

public:

	virtual HRESULT Initialize(void* pArg)override;
	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta)override;
	virtual void Exit_State()  override;

	virtual void OnNotify(const EVENT& eEvent) override;
private:
	void		KickDoor(shared_ptr<CBoss_Teacher> pTeacher, const _float& fTimeDelta);
	void		Turn(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform);
	void		SMesh_Generator(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform);
	void		StandBy(shared_ptr<CBoss_Teacher>pBoss, shared_ptr<CTransform> pTransform);
	void		Chase(shared_ptr<CNavigation> pNavi,shared_ptr<CBoss_Teacher>pBoss,shared_ptr<CTransform> pTransform, const _float& fTimeDelta);
private:
	uint32_t		m_iNaviEventIndex{};


	_float3			m_DestPos;
public:
	static unique_ptr<CFSM_Teacher_Spawn> Create(ComPtr<ID3D11Device>pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype> Clone(void* pArg);
};


NS_END;

