#pragma once
#include "Boss_Teacher.h"

NS_BEGIN(Engine)
class  CTeacher_FSM : public CFSM_STATE
{
public:
	typedef struct strTeacherFsm
	{
		weak_ptr<CBoss_Teacher>		pTeacher;
		_float4x4*					pOtherMatrix;
	}FSM_TEACHER_DESC;
protected:
	CTeacher_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTeacher_FSM(const CTeacher_FSM& Prototype);
public:
	virtual ~CTeacher_FSM();

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Initialize_State(weak_ptr<CGameObject> pObj) override;
	virtual void	 Enter_State() PURE;
	virtual void	 Update_State(_float fTimeDelta)PURE;
	virtual void	 Exit_State()  PURE;
	void					Move(const _float& fTimeDelta, MOVE eMove, shared_ptr<class CTransform>& pTransform, shared_ptr<class CNavigation>& pNavigation);

public:
	void					Set_Boss(weak_ptr<CBoss_Teacher> pPlayer) { m_pBoss = pPlayer; }
protected:
	_float4x4*							m_pOtherMatrix;
	weak_ptr<CBoss_Teacher>				m_pBoss;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};


NS_END;