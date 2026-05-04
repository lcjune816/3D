#include "Layer.h"
#include "GameInstance.h"
CLayer::CLayer()
{
}

CLayer::~CLayer()
{

}


HRESULT CLayer::Add_GameObject(shared_ptr<CGameObject> pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}
void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != *iter)
		{
			if ((*iter)->Get_Dead())
			{
				(*iter).reset();
				iter = m_GameObjects.erase(iter);
				continue;
			}
			(*iter)->Priority_Update(fTimeDelta);

		}
		++iter;
	}
		
}
void CLayer::Update(_float fTimeDelta)
{

	for (auto& iter : m_GameObjects)
	{
		if(nullptr != iter)
		iter->Update(fTimeDelta);
	}
}
void CLayer::Late_Update(_float fTimeDelta)
{

	for (auto& iter : m_GameObjects)
	{
		if (NULL_FALSE(iter))
		{
			iter->Late_Update(fTimeDelta);
		}
			
	}
}

void CLayer::CopyLater_Late_Update(_float fTimeDelta)
{
	for (auto& iter : m_GameObjects)
	{
		if (NULL_FALSE(iter))
		{
			iter->Late_Update(fTimeDelta);
		}
	}
}

void CLayer::MoveToLayer(shared_ptr<CGameObject> pObj)
{
	if (NULL_TRUE(pObj))
		return;

	m_GameObjects.emplace_back(pObj);
}

void CLayer::Clear_ObjectList()
{
	m_GameObjects.clear();
}

unique_ptr<CLayer> CLayer::Create()
{
	return unique_ptr<CLayer>(new CLayer());
}