#include "Event_Manager.h"
#include "GameInstance.h"

CEvent_Manager::CEvent_Manager()
{

}
CEvent_Manager::~CEvent_Manager()
{

}

HRESULT CEvent_Manager::Initialize()
{


    return S_OK;
}
void CEvent_Manager::Notify(const WORLD_EVENT& eEvent, const EVENT& event)
{
    if (m_Observers[eEvent].empty())
        return;

    for (auto& List : m_Observers[eEvent])
    {
        auto pObserver = List.lock();
        if (NULL_TRUE(pObserver))
            continue;

        pObserver->OnNotify(event);
    }
    m_Observers[eEvent].clear();
}
HRESULT CEvent_Manager::Add_Observers(const WORLD_EVENT& eEvent, shared_ptr<CObserver> pObserver)
{
    
    m_Observers[eEvent].push_back(pObserver);
    return S_OK;
}

unique_ptr<CEvent_Manager> CEvent_Manager::Create()
{
    auto pInstance = unique_ptr<CEvent_Manager>(new CEvent_Manager());

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed Light_Manager");
        return nullptr;
    }

    return pInstance;
}