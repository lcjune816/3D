#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CImGuiManager final
{
private:
	CImGuiManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CImGuiManager();

public:
	HRESULT					Initialize(HWND hWnd);
	HRESULT					Update();
	void					Render();

public:
	_bool					WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	void					Add_FilePath(const string fileName, const string filePath);
	HRESULT					File_Loader();
	const string&			Find_Path(const string& strFileName);

	const vector<string>&  Get_FileNameList() { return m_NameList; }
public:

	static unique_ptr<CImGuiManager> Create(HWND hWnd, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
private:

	unordered_map<string, string>		m_PathList;
	vector<string>						m_NameList;

	ComPtr<ID3D11Device>				m_pDevice;
	ComPtr<ID3D11DeviceContext>			m_pContext;

};

NS_END
