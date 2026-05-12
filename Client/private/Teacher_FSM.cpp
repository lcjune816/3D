#include "Teacher_FSM.h"

CTeacher_FSM::CTeacher_FSM(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CFSM_STATE(pDevice, pContext)
{
}

CTeacher_FSM::CTeacher_FSM(const CTeacher_FSM& Prototype) :CFSM_STATE(Prototype)

{
}

CTeacher_FSM::~CTeacher_FSM()
{
}

HRESULT CTeacher_FSM::Initialize(void* pArg)
{
	auto pDesc = static_cast<FSM_TEACHER_DESC*>(pArg);
	
	m_pBoss = pDesc->pTeacher;
	m_pOtherMatrix = pDesc->pOtherMatrix;
	return S_OK;
}

HRESULT CTeacher_FSM::Initialize_State(weak_ptr<CGameObject> pObj)
{
	auto Owner = pObj.lock();
	if (NULL_TRUE(Owner))
		return E_FAIL;

	m_pBoss = static_pointer_cast<CBoss_Teacher>(Owner);
	m_pOtherMatrix = m_pBoss.lock()->Get_OtherMatrixPtr();
	return S_OK;
}