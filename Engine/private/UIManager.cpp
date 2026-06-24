#include "UIManager.h"
#include "GameInstance.h"
#include "UIObject.h"
CUIManager::CUIManager()
{

}

CUIManager::~CUIManager()
{

}


HRESULT	 CUIManager::Initialize(uint32_t iLevelIndex)
{
	m_UIObjectss.resize(iLevelIndex);
	m_iLevel = iLevelIndex;
	return S_OK;
}
void CUIManager::Add_UI(uint32_t iLevelIndex, shared_ptr<CUIObject> pUI)
{
	m_UIObjectss[iLevelIndex].push_back(pUI);

}

void CUIManager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevel; ++i)
	{
		for (auto& iter : m_UIObjectss[i])
			iter->Priority_Update(fTimeDelta);
	}
}
void CUIManager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevel; ++i)
	{
		for (auto& iter : m_UIObjectss[i])
			iter->Update(fTimeDelta);
	}
}
void CUIManager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevel; ++i)
	{
		for (auto& iter : m_UIObjectss[i])
			iter->Late_Update(fTimeDelta);
	}
}

void CUIManager::Clear(uint32_t iLevelIndex)
{
	m_UIObjectss[iLevelIndex].clear();
}
unique_ptr<CUIManager> CUIManager::Create(uint32_t iLevelIndex)
{
	auto pInstance = unique_ptr<CUIManager>(new CUIManager);
	if (FAILED(pInstance->Initialize(iLevelIndex)))
	{
		MSG_BOX("Create Failed Trigger Manager");
		return nullptr;
	}

	return pInstance;
}
