#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CEvent_Manager final
{

private:
	CEvent_Manager();
public:
	~CEvent_Manager();

public:
	HRESULT Initialize();

	void				  Notify(const WORLD_EVENT& eEvent, const EVENT& event);
	HRESULT				  Add_Observers(const WORLD_EVENT& eEvent, shared_ptr<class CObserver> pObserver);
	

private:
	map<WORLD_EVENT, vector<weak_ptr<class CObserver> >>		m_Observers;
public:
	static unique_ptr<CEvent_Manager> Create();

};

NS_END

