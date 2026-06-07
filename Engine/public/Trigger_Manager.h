#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CTrigger_Manager final
{
private:
	CTrigger_Manager();
public:
	virtual ~CTrigger_Manager();

public:
	HRESULT								Initialize(uint32_t iLevelIndex);
	HRESULT								 Add_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber, weak_ptr<class CTrigger> pTrigger);
	weak_ptr<CTrigger>					 Find_Trigger(uint32_t iLevelIndex, uint32_t iTargetNumber);
	void								 Clear(uint32_t iLevelIndex);
private:
	vector<map<uint32_t, weak_ptr<class CTrigger>>>		m_Triggers;

public:
	static unique_ptr<CTrigger_Manager> Create(uint32_t iLevelIndex);
};

NS_END
