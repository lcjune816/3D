#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CUIManager final
{
private:
	CUIManager();
public:
	virtual ~CUIManager();

public:
	HRESULT								Initialize(uint32_t iLevelIndex);
	void								 Add_UI(uint32_t iLevelIndex, shared_ptr<class CUIObject> pUI);
	void								 Clear(uint32_t iLevelIndex);

public:

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
private:
	vector<list<shared_ptr<class  CUIObject>>>		m_UIObjectss;

	uint32_t							m_iLevel;
public:
	static unique_ptr<CUIManager> Create(uint32_t iLevelIndex);
};

NS_END
