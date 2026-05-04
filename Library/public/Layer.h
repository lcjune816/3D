#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CLayer
{
private:
	CLayer();
public:
	~CLayer();

public:
	HRESULT Add_GameObject(shared_ptr<CGameObject> pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void CopyLater_Late_Update(_float fTimeDelta);
	void	MoveToLayer(shared_ptr<CGameObject> pObj);
	void	Clear_ObjectList();
	list<shared_ptr<CGameObject>>& Get_ObjectList() { return m_GameObjects; }


private:
	list<shared_ptr<CGameObject>> m_GameObjects;
	
	
public:
	static unique_ptr<CLayer> Create();
};

NS_END

