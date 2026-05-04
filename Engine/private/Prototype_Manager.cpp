#include "Prototype_Manager.h"

CPrototype_Manager::CPrototype_Manager()
{

}
CPrototype_Manager::~CPrototype_Manager()
{

}

HRESULT		CPrototype_Manager::Initialize(uint32_t iNumLevels)
{
	m_pPrototypes = unique_ptr<PROTOTYPES[]>(new PROTOTYPES[iNumLevels]);

	m_iNumLevels = iNumLevels;
	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, unique_ptr<CPrototype> pPrototype)
{
	if (NULL_FALSE(Find_Prototype(iLevelIndex, strPrototypeTag)))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, std::move(pPrototype));

	return S_OK;
}

shared_ptr<CPrototype> CPrototype_Manager::Clone_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CPrototype* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (NULL_TRUE(pPrototype))
		return nullptr;

	return pPrototype->Clone(pArg);
}

CPrototype* CPrototype_Manager::Find_Prototype(uint32_t iLevelIndex, const _wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second.get();
}
void CPrototype_Manager::Clear(uint32_t iClearLevelIndex)
{
	if (iClearLevelIndex >= m_iNumLevels || NULL_TRUE(m_pPrototypes))
		return;

	for (auto& Pair : m_pPrototypes[iClearLevelIndex])
	{
		Pair.second.reset();
	}
	m_pPrototypes[iClearLevelIndex].clear();
}

unique_ptr<CPrototype_Manager> CPrototype_Manager::Create(uint32_t iNumLevels)
{

	auto pInstance = unique_ptr<CPrototype_Manager>(new CPrototype_Manager());
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Create Failed CProto_Manager");
		return nullptr;
	}

	return pInstance;
}