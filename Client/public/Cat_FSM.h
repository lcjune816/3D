#pragma once
#include "Boss_Teacher.h"
#include "Cat.h"
NS_BEGIN(Client)
enum class FSM_CAT {
	KICK, SMASH, CHASE, TURN,
	END
};
class  CCat_FSM : public CFSM_STATE
{
public:
	typedef struct strCatrFsm
	{
		_float4x4* pOtherMatrix;
	}FSM_CAT_DESC;
protected:
	CCat_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCat_FSM(const CCat_FSM& Prototype);
public:
	virtual ~CCat_FSM();

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Initialize_State(weak_ptr<CGameObject> pObj) override;
	virtual void	 Enter_State() PURE;
	virtual void	 Update_State(_float fTimeDelta)PURE;
	virtual void	 Exit_State()  PURE;
	
protected:
	_float4x4*							 m_pOtherMatrix;
	_float								m_fTick{ 0 }, m_fTimeCnt{ 0 };

	FSM_CAT								m_eCat;
	FSM_ACTION							m_eAction;

	weak_ptr<CBoss_Cat>					m_pBoss;
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};


NS_END;