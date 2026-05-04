#pragma once
#include "GuiObject.h"

class ENGINE_DLL GuiTile : public GuiObject
{
private: 
	GuiTile() = default;
public:
	~GuiTile();

public:
	HRESULT		Initialize()	override;
	HRESULT		Update()		override;
	void		Render()	    override;

public:
	string			Select_Model(vector<FPATH>& pPath);
	INFO			Setup_Info();
	
	string		    ModelLoader(const vector<string>& pName = {});
	void			ImGui_Gizmo();

private:
	INFO				m_tInfo = {};
public:
	static unique_ptr<GuiTile> Create();


};

