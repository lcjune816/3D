#include "ImGuiManager.h"

CImGuiManager::CImGuiManager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : m_pDevice{ pDevice }, m_pContext{ pContext }
{

}
CImGuiManager::~CImGuiManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImPlot3D::DestroyContext();
	ImGui::DestroyContext();
}

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT CImGuiManager::Initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot3D::CreateContext();
	//ImGui_ImplWin32_EnableDpiAwareness();
	//SetProcessDPIAware();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

	ImFont* Font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/malgun.ttf", 16.f, NULL, io.Fonts->GetGlyphRangesKorean());


	File_Loader();
	return S_OK;
}

HRESULT CImGuiManager::Update()
{
	_bool test = true;
	string path = {};
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame(); 
	//ImGuizmo::BeginFrame();
	
	//ImGui::ShowDemoWindow(&test);
	//ImPlot3D::ShowDemoWindow(&test);
	

	
	return S_OK;

}
void CImGuiManager::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	

}

unique_ptr<CImGuiManager> CImGuiManager::Create(HWND hWnd, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CImGuiManager> pInstance = unique_ptr<CImGuiManager>(new CImGuiManager(pDevice,pContext));

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		return nullptr;
	}

	return pInstance;
}

_bool CImGuiManager::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!ImGui::GetCurrentContext()) return false;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

		return false;
}

void CImGuiManager::Add_FilePath(const string fileName, const string filePath)
{
	auto iter = m_PathList.find(fileName);
	if (iter == m_PathList.end())
	{
		m_PathList.emplace(fileName, filePath);
	}
}

HRESULT CImGuiManager::File_Loader()
{
	string path = "../../Resource";
	
	//이거 혁명인데요..
	for (auto& iter : filesystem::recursive_directory_iterator(path))
	{
		if (iter.path().extension() == ".fbx")
		{
			string strFileName = iter.path().filename().string();
			string strPathName = iter.path().string();
			m_PathList.emplace(strFileName, strPathName);
			m_NameList.push_back(strFileName);
		}
		else if(iter.path().extension() == ".bin")
		{
			string strFileName = iter.path().filename().string();
			string strPathName = iter.path().string();
			m_PathList.emplace(strFileName, strPathName);
			m_NameList.push_back(strFileName);
		}
	}

	return S_OK;
}

const string& CImGuiManager::Find_Path(const string& strFileName)
{
	
	auto iter  = m_PathList.find(strFileName);
	static const string failed = "";
	if (iter != m_PathList.end())
	{
		return iter->second;
	}

	return failed;
}
