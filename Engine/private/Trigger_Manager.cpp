#include "Trigger_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
CTrigger_Manager::CTrigger_Manager()
{

}

CTrigger_Manager::~CTrigger_Manager()
{

}


HRESULT	 CTrigger_Manager::Initialize(uint32_t iLevelIndex)
{
	m_Triggers.resize(iLevelIndex);
	return S_OK;
}
HRESULT CTrigger_Manager::Add_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber, weak_ptr<CTrigger> pTrigger)
{
	auto iter = m_Triggers[iLevelIndex].find(iTargetNumber);

	if (iter == m_Triggers[iLevelIndex].end())
	{
		m_Triggers[iLevelIndex].emplace(iTargetNumber, pTrigger);
		return S_OK;
	}

	return E_FAIL;
		
}

weak_ptr<CTrigger> CTrigger_Manager::Find_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber)
{
	auto iter = m_Triggers[iLevelIndex].find(iTargetNumber);

	if (iter != m_Triggers[iLevelIndex].end())
	{
		if (nullptr != iter->second.lock())
		{
			return iter->second;
		}
	}

	return {};
}
void CTrigger_Manager::Clear(uint32_t iLevelIndex)
{
	m_Triggers[iLevelIndex].clear();
}
unique_ptr<CTrigger_Manager> CTrigger_Manager::Create(uint32_t iLevelIndex)
{
	auto pInstance = unique_ptr<CTrigger_Manager>(new CTrigger_Manager);
	if (FAILED(pInstance->Initialize(iLevelIndex)))
	{
		MSG_BOX("Create Failed Trigger Manager");
		return nullptr;
	}

	return pInstance;
}
