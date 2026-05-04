#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL GuiObject final
{
private:
	GuiObject() = default;
public:
	~GuiObject();

public:
	HRESULT				Initialize();
	HRESULT				Update();
	void				Render();

public:
	HRESULT				Enable_GUI();
	string				Select_Model(const vector<FPATH>& pPath);
	string				Model_Animation(const vector<string>& pNames);


	void				ImGui_Gizmo();

private:
	INFO				m_tInfo = {};
public:
	static unique_ptr<GuiObject> Create();


};

NS_END