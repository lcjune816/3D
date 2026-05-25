#include "GameObject.h"
#include "GameInstance.h"
#include "Generator.h"
CGenerator::CGenerator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : CTrigger{ pDevice, pContext }
{
}

CGenerator::CGenerator(const CGenerator& Prototype) : CTrigger(Prototype)
{
}

CGenerator::~CGenerator()
{
}

HRESULT CGenerator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGenerator::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	m_eEventTrigger = TRIGGER_EVENT::GENERATOR;
	m_eState = TRIGGER_STATE::IDLE;
	Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::ENABLE);

	CGameInstance::Get().Add_Observers(WORLD_EVENT::GENERATOR, SHARED_THIS(CGenerator));
	return S_OK;
}

HRESULT CGenerator::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		Action_Work(fTimeDelta);
		break;
	case TRIGGER_STATE::RETURN:
		
		break;

	case TRIGGER_STATE::PAUSE:

		break;
	}
	return S_OK;
}
HRESULT CGenerator::Late_Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	return S_OK;
}

void CGenerator::TriggerToTrigger()
{
	if (Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
	{
		m_eState = TRIGGER_STATE::ACTION;
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
	}
}

void CGenerator::Set_Trigger()
{
}

void CGenerator::OnNotify(const EVENT& event)
{
	auto pParent = m_pParent.lock();
	if (NULL_TRUE(pParent))
		return;
	auto transform = pParent->Get_Transform().lock();

	vector<uint32_t> MeshNameList;
	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/SM_NormalGenerator_B.bin";
	importModel.bAllModel = 0;
	importModel.eType = MESH_TYPE::TRIGGER;
	CGameInstance::Get().ImportModel_NonAnime(importModel, transform , MeshNameList);
	pParent->Mesh_Change(MeshNameList);

}

void CGenerator::Action_Work(const _float& fTimeDelta)
{
	m_fFrameTick += fTimeDelta;

	if (m_fFrameTick > 1.f)
	{
		++m_fFrameTickTime;
	}

	if (m_fFrameTickTime >= 3)
	{
		auto pObj = m_pParent.lock();
		if (NULL_TRUE(pObj))
			return;
		
		auto pTransform = pObj->Get_Transform().lock();
		if (NULL_TRUE(pTransform))
			return;

		_float3 fPos = {};
		XMStoreFloat3(&fPos, pTransform->Get_State(STATE::POS));
		EVENT pArg;
		pArg.pArg = &fPos;
		CGameInstance::Get().Notify(WORLD_EVENT::TEACHER_SPAWN, pArg);

		m_eState = TRIGGER_STATE::PAUSE;
	}
}

void CGenerator::Action_Trigger()
{
}

unique_ptr<CGenerator>CGenerator::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CGenerator>(new CGenerator(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CGenerator");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CGenerator::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CGenerator>(new CGenerator(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CGenerator Clone");
		return nullptr;
	}

	return pInstance;

}