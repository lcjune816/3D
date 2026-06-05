#include "Cat_FSM.h"

CCat_FSM::CCat_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CFSM_STATE(pDevice, pContext)
{
}

CCat_FSM::CCat_FSM(const CCat_FSM& Prototype) :CFSM_STATE(Prototype)

{
}

CCat_FSM::~CCat_FSM()
{
}

HRESULT CCat_FSM::Initialize(void* pArg)
{
	auto pDesc = static_cast<FSM_CAT_DESC*>(pArg);

	return S_OK;
}

HRESULT CCat_FSM::Initialize_State(weak_ptr<CGameObject> pObj)
{
	auto Owner = pObj.lock();
	if (NULL_TRUE(Owner))
		return E_FAIL;
	m_pBoss = static_pointer_cast<CBoss_Cat>(Owner);
	return S_OK;
}
