#include "Trigger_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
CTrigger_Manager::CTrigger_Manager()
{

}

CTrigger_Manager::~CTrigger_Manager()
{

}


HRESULT CTrigger_Manager::Add_Trigger(uint32_t iTargetNumber, weak_ptr<CTrigger> pTrigger)
{
	auto iter = m_Triggers.find(iTargetNumber);

	if (iter == m_Triggers.end())
	{
		m_Triggers.emplace(iTargetNumber, pTrigger);
		return S_OK;
	}

	return E_FAIL;
		
}

weak_ptr<CTrigger> CTrigger_Manager::Find_Trigger(uint32_t iTargetNumber)
{
	auto iter = m_Triggers.find(iTargetNumber);

	if (iter != m_Triggers.end())
	{
		if (nullptr != iter->second.lock())
		{
			return iter->second;
		}
		else
			m_Triggers.erase(iter);
	}

	return {};
}
void CTrigger_Manager::Clear()
{
	m_Triggers.clear();
}
unique_ptr<CTrigger_Manager> CTrigger_Manager::Create()
{
	return unique_ptr<CTrigger_Manager>(new CTrigger_Manager);
}
