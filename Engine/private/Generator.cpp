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
	m_fFrameTick = 0.f;
	return S_OK;
}

HRESULT CGenerator::Interaction(_float fTimeDelta, _bool bOtherTrigger)
{
	_float fVolume{};
	switch (m_eState)
	{
	case TRIGGER_STATE::IDLE:
		break;
	case TRIGGER_STATE::ACTION:
		Action_Work(fTimeDelta);
		break;
	case TRIGGER_STATE::RETURN:

		m_fFrameTick += fTimeDelta;
		fVolume = max(0.f, 0.2f - (m_fFrameTick / 3.f) * 0.2f);
		if (fVolume <= 0.f)
		{
			STOP_SOUND(CHANNELID::SOUND_WORLDEVENT);
		}

		VOLCTL(CHANNELID::SOUND_WORLDEVENT, fVolume);
		m_eState = TRIGGER_STATE::PAUSE;
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
		STOP_SOUND(CHANNELID::SOUND_BGM01);
		PLAY_SOUND(GENERATOR_POWERON_SOUND, CHANNELID::SOUND_WORLDEVENT, 0.05f);
		m_eState = TRIGGER_STATE::ACTION;
		Set_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER), FLAGVALUE::DISABLE);
		EVENT eEvent{};
		PLAY_SOUND(TEACHER_BGM_SOUND2, CHANNELID::SOUND_BGM01, 0.2f);
		eEvent.eEvent = WORLD_EVENT::BOSS_LIGHT_OFF;
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_LIGHT_OFF, eEvent);
		eEvent.eEvent = WORLD_EVENT::BOSS_LIGHT_FLICK;
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_LIGHT_FLICK, eEvent);
	}
}

void CGenerator::Set_Trigger()
{
}

void CGenerator::OnNotify(const EVENT& event)
{
	STOP_SOUND(CHANNELID::SOUND_WORLDEVENT);
	PLAY_SOUND(GENERATOR_BREAK, CHANNELID::SOUND_WORLDEVENT, 0.1f);
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
	m_eState = TRIGGER_STATE::RETURN;
}

void CGenerator::Action_Work(const _float& fTimeDelta)
{
	m_fFrameTick += fTimeDelta;

	IS_PLAYSOUND(GENERATOR_POWERON_LOOP, CHANNELID::SOUND_WORLDEVENT, 0.05f);
	if (m_fFrameTick > 1.f)
	{
		m_fFrameTick = 0.f;
		++m_fFrameTickTime;
	}

	if (m_fFrameTickTime >= 3.f)
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
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_SPAWN, pArg);
		m_fFrameTick = 0.f;
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