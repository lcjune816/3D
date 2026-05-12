#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CObject_Manager final
{
private:
	CObject_Manager();
public:
	virtual ~CObject_Manager();

public:
	HRESULT Initialize(uint32_t iNumLevels);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Clear(uint32_t iClearLevelIndex);
public:
	HRESULT Add_GameObject_ToLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag,
	uint32_t iLayerLevelIndex, const _wstring& strLayerTag, void* pArg);

	HRESULT Resize_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag ,int32_t iResize);


	HRESULT Add_GameObject_ToCopyLayer(uint32_t iPrototypeLevelIndex, const _wstring strPrototypeTag, void* pArg);
	HRESULT Move_CopyLayer_ToObjectLayer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag);
	void	Move_To_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _wstring& strNextLayerTag, shared_ptr<class CGameObject> pObj);
	void    Move_Tol_AllLayer(uint32_t iLayerCurrentLevelIndex, const _wstring& strNextLayerTag, list<shared_ptr<class CGameObject>> pObj);
	
	CGameObject* Get_ObjectPtr(uint32_t iLayerCurrentLevelIndex, const _wstring& strCurrentLayerTag, const _char* ObjTag);
	void	Add_Layer(uint32_t iLayerCurrentLevelIndex, const _wstring& strLayerTag);
	void	Reset_CopyLayer();

	void	   Save_Data(uint32_t iNumLevel, _wstring path, const _wstring& strLayerName, const string& pSaveArrayName);
private:
	uint32_t		m_iNumLevels = {};

private:
	unique_ptr<map<const _wstring, unique_ptr<class CLayer>>[]> m_pLayers;
		typedef map<const _wstring, unique_ptr<class CLayer>> LAYERS;

		unique_ptr<class CLayer>								m_pCopyLayer;
public:
	class CLayer* Find_Layer(uint32_t iLayerLevelIndex, const _wstring& strLayerTag);

public:
	static unique_ptr<CObject_Manager> Create(uint32_t iNumLevels);
};

NS_END
