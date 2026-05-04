#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CLevel_Manager final
{
private:
    CLevel_Manager();

public:
    ~CLevel_Manager();

public:
    HRESULT Change_Level(uint32_t iNewLevelIndex, unique_ptr<class CLevel> pNewLevel);
    void Update(_float fTimeDelta);
    HRESULT Render();

private:
    uint32_t                m_iCurrentLevelIndex = {};
    unique_ptr<class CLevel>        m_pCurrentLevel = { nullptr };

public:
    static unique_ptr<CLevel_Manager> Create();
};

NS_END
