#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
{

}

CLevel_Manager::~CLevel_Manager()
{
}

HRESULT CLevel_Manager::Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel)
{
	if (nullptr == pNewLevel)
		return E_FAIL;

	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance::Get().Clear_Resources(m_iCurrentLevelIndex);
		m_pCurrentLevel.reset();

	}


	m_pCurrentLevel = std::move(pNewLevel);

	m_iCurrentLevelIndex = iNewLevelIndex;

	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;
	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

unique_ptr<CLevel_Manager> CLevel_Manager::Create()
{
	return unique_ptr<CLevel_Manager>(new CLevel_Manager());
}
