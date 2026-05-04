#include "Object_Manager.h"
#include "Layer.h"
#include  "GameObject.h"
#include "GameInstance.h"
CObject_Manager::CObject_Manager()
{

}

CObject_Manager::~CObject_Manager()
{

}

HRESULT CObject_Manager::Initialize(uint32_t iNumLevels)
{
	m_pLayers = unique_ptr<LAYERS[]>(new LAYERS[iNumLevels]);
	
	m_iNumLevels = iNumLevels;

	m_pCopyLayer = CLayer::Create();

	return S_OK;
}
void    CObject_Manager::Priority_Update(_float fTimeDelta)
{
	
	m_pCopyLayer->Priority_Update(fTimeDelta);

	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->Priority_Update(fTimeDelta);
		}
	}

}
void    CObject_Manager::Update(_float fTimeDelta)
{
	m_pCopyLayer->Update(fTimeDelta);

	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->Update(fTimeDelta);
		}
	}

}
	
void    CObject_Manager::Late_Update(_float fTimeDelta)
{
	m_pCopyLayer->CopyLater_Late_Update(fTimeDelta);

	for (uint32_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->Late_Update(fTimeDelta);
		}
	}
	
}
void CObject_Manager::Move_Tol_AllLayer(uint32_t iLayerCurrentLevelIndex,  const _wstring& strNextLayerTag, list<shared_ptr<CGameObject>> pObj)
{
	auto pLayer = Find_Layer(iLayerCurrentLevelIndex, strNextLayerTag);

	if (NULL_TRUE(pLayer))
	{
		auto Layer = CLayer::Create();
	
		auto& LayerList = Layer->Get_ObjectList();

		LayerList = move(pObj);

		m_pLayers[iLayerCurrentLevelIndex].emplace(strNextLayerTag,move(Layer));
		return;
	}

	auto& Layer = pLayer->Get_ObjectList();

	Layer = move(pObj);

	m_pLayers[iLayerCurrentLevelIndex].emplace(strNextLayerTag, move(pLayer));

}
void CObject_Manager::Clear(uint32_t iClearLevelIndex)
{
	if (iClearLevelIndex >= m_iNumLevels ||
		nullptr == m_pLayers)
		return;

	for (auto& Pair : m_pLayers[iClearLevelIndex])
	{
		Pair.second.reset();
	}
	m_pLayers[iClearLevelIndex].clear();
}
HRESULT CObject_Manager::Add_GameObject_ToLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag,
	uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	if (nullptr == m_pLayers || iLayerLevelIndex >= m_iNumLevels)
		return E_FAIL;

	auto	pObj = dynamic_pointer_cast<CGameObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pObj)
		return E_FAIL;
	

	auto     pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		auto		pNewLayer = CLayer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;

		if (FAILED(pNewLayer->Add_GameObject(pObj)))
			return E_FAIL;
		
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
	}
	else
		pLayer->Add_GameObject(pObj);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_ToCopyLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag, void* pArg)
{
	auto pObj = static_pointer_cast<CGameObject>(CGameInstance::Get().Clone_Prototype(iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (NULL_TRUE(pObj))
		return E_FAIL;
	pObj->Set_CheckCopyLater(true);
	m_pCopyLayer->Add_GameObject(pObj);
	return S_OK;
}

HRESULT CObject_Manager::Move_CopyLayer_ToObjectLayer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (NULL_TRUE(pLayer))
		return E_FAIL;

	for (auto& Copy : m_pCopyLayer->Get_ObjectList()) 
	{
		
		if (NULL_FALSE(Copy.get()))
		{
			Copy->Set_CheckCopyLater(false);
			pLayer->MoveToLayer(move(Copy));
		}
	}

	return S_OK;
}

void CObject_Manager::Move_To_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _wstring& strNextLayerTag, shared_ptr<CGameObject> pObj)
{
	auto Layer = Find_Layer(iLayerCurrentLevelIndex, strCurrentLayerTag);
	auto MoveLayer = Find_Layer(iLayerCurrentLevelIndex, strNextLayerTag);

	if (NULL_TRUE(Layer) || NULL_TRUE(MoveLayer))
		return;

	MoveLayer->Add_GameObject(pObj);
	auto& ObjList = Layer->Get_ObjectList(); 

	ObjList.erase(remove(ObjList.begin(),ObjList.end(),pObj));
}

void CObject_Manager::Add_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strLayerTag)
{
	auto Layer = Find_Layer(iLayerCurrentLevelIndex, strLayerTag);
	if (NULL_FALSE(Layer))
		return;

	auto pLayer = CLayer::Create();

	if (NULL_TRUE(pLayer))
		return;
	
	m_pLayers[iLayerCurrentLevelIndex].emplace(strLayerTag, move(pLayer));

}

void CObject_Manager::Reset_CopyLayer()
{
	m_pCopyLayer->Clear_ObjectList();
}

HRESULT CObject_Manager::Resize_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag, int32_t iResize)
{
	auto pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		auto		pNewLayer = CLayer::Create();
		if (nullptr == pNewLayer)
			return E_FAIL;

		pNewLayer->Get_ObjectList().resize(iResize);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, std::move(pNewLayer));
		return S_OK;
	}

	pLayer->Get_ObjectList().resize(iResize);

	return S_OK;
}


void  CObject_Manager::Save_Data(uint32_t iNumLevel, _wstring path, const _wstring& strLayerName, const string& pSaveArrayName)
{
	CLayer* pLayer = Find_Layer(iNumLevel, strLayerName);

	if (nullptr == pLayer)
	{
		MSG_BOX("Layer Null");
		return;
	}
	json j;
	for (auto& iter : pLayer->Get_ObjectList())
	{
		if (iter->Save_Data() == nullptr)
			continue;

		j[pSaveArrayName].push_back(iter->Save_Data());
	}
	
	ofstream file(path);
	file << j.dump(4);

	file.close();

	MSG_BOX("µÈµí?");
}

CLayer* CObject_Manager::Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag)
{
	if (iLayerLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second.get();
}
unique_ptr<CObject_Manager> CObject_Manager::Create(uint32_t iNumLevels)
{
	auto		pInstance = unique_ptr<CObject_Manager>(new CObject_Manager());

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		return nullptr;
	}

	return pInstance;
}