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
	HRESULT								 Add_Trigger(uint32_t iTargetNumber, weak_ptr<class CTrigger> pTrigger);
	weak_ptr<CTrigger>					 Find_Trigger(uint32_t iTargetNumber);
private:
	map<uint32_t, weak_ptr<class CTrigger>>		m_Triggers;

public:
	static unique_ptr<CTrigger_Manager> Create();
};

NS_END
