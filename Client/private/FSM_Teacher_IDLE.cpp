#include "FSM_Teacher_IDLE.h"
CFSM_Teacher_IDLE::CFSM_Teacher_IDLE(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CTeacher_FSM(pDevice, pContext)
{
}

CFSM_Teacher_IDLE::CFSM_Teacher_IDLE(const CFSM_Teacher_IDLE& Prototype) :CTeacher_FSM(Prototype)
{
}

CFSM_Teacher_IDLE::~CFSM_Teacher_IDLE()
{
}
HRESULT CFSM_Teacher_IDLE::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}
void CFSM_Teacher_IDLE::Enter_State()
{

	auto Boss = m_pBoss.lock();

	if (NULL_TRUE(Boss))return;

	Boss->GetAnimator()->Stop_Animation(true);
}

void CFSM_Teacher_IDLE::Update_State(_float fTimeDelta)
{

}

void CFSM_Teacher_IDLE::Exit_State()
{
	auto Boss = m_pBoss.lock();

	if (NULL_TRUE(Boss))return;
	Boss->GetAnimator()->Stop_Animation(false);
}

unique_ptr<CFSM_Teacher_IDLE>		CFSM_Teacher_IDLE::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Teacher_IDLE>(new CFSM_Teacher_IDLE(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Teacher_IDLE::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Teacher_IDLE>(new CFSM_Teacher_IDLE(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}