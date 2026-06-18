#include "MainApp.h"
#include "ImGuiManager.h"
#include "GameInstance.h"
#include "Loader_Defines.h"
#include "Level_Loading.h"
#include "UILoadingScreen.h"
#include <assimp/Importer.hpp>
Client::CMainApp::CMainApp()
{
}

Client::CMainApp::~CMainApp()
{

	//D3D11_BLEND_OP_MIN = 4,
	//D3D11_BLEND_OP_MAX = 5
	//D3D11_BLEND_ZERO = 1,
	//D3D11_BLEND_ONE = 2,
	//D3D11_BLEND_SRC_COLOR = 3,
	//D3D11_BLEND_INV_SRC_COLOR = 4,
	//D3D11_BLEND_SRC_ALPHA = 5,
	//D3D11_BLEND_INV_SRC_ALPHA = 6,
	//D3D11_BLEND_DEST_ALPHA = 7,
	//D3D11_BLEND_INV_DEST_ALPHA = 8,
	//D3D11_BLEND_DEST_COLOR = 9,
	//D3D11_BLEND_INV_DEST_COLOR = 10,
	//D3D11_BLEND_SRC_ALPHA_SAT = 11,
	//D3D11_BLEND_BLEND_FACTOR = 14,
	//D3D11_BLEND_INV_BLEND_FACTOR = 15,
	//D3D11_BLEND_SRC1_COLOR = 16,
	//D3D11_BLEND_INV_SRC1_COLOR = 17,
	//D3D11_BLEND_SRC1_ALPHA = 18,
	//D3D11_BLEND_INV_SRC1_ALPHA = 19
	CGameInstance::Get().Release_Engine();
}

HRESULT Client::CMainApp::Initialize()
{
	//D3D11_SAMPLER_DESC
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ETOUI(LEVEL::END);
	EngineDesc.hInstance = g_hInstance;
	if (FAILED(CGameInstance::Get().Initialize_Engine(EngineDesc, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;


	return S_OK;
}

void Client::CMainApp::Update(float fTimeDelta)
{
	CGameInstance::Get().Update_Engine(fTimeDelta);
	//ShowCursor(false);
}

HRESULT Client::CMainApp::Render()
{
	const _float4 fColor = { 0.f,0.f,1.f,1.f };
	// RenderTarget ¼³Á¤
	CGameInstance::Get().Clear_DepthStencil_View();
	CGameInstance::Get().Clear_BackBuffer_View(&fColor);

	CGameInstance::Get().Draw();

	CGameInstance::Get().Present();
	return S_OK;
}

HRESULT Client::CMainApp::Ready_Prototype()
{
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), 
		TEXT("Component_NonAnime"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/VertexShader.hlsl"),
		VERTEX_NOANIME::Elements, VERTEX_NOANIME::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Component_Box"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/BoxShader.hlsl"),
			VERTEX_BOX::Elements, VERTEX_BOX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Component_Animation"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/AnimeShader.hlsl"),
			VERTEX::Elements, VERTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Component_ArrayShader"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Array_VtxTex.hlsl"),
			VERTEX_NOANIME::Elements, VERTEX_NOANIME::iNumElements))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Component_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
			VTX_TEX::Elements, VTX_TEX::iNumElements))))
		return E_FAIL;
	

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Component_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/UIShader.hlsl"),
			VTX_TEX::Elements, VTX_TEX::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),TEXT("Component_Instancing_NonAnime"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInsatncing_Mesh.hlsl"),
			VTXINSTANCE_NONANIME_MESH::Elements, VTXINSTANCE_NONANIME_MESH::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),TEXT("Component_Instancing_Fog"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInsatncing_Fog.hlsl"),
			VTXINSTANCE_PARTICLE_POINT::Elements, VTXINSTANCE_PARTICLE_POINT::iNumElements))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),TEXT("Component_Instancing_Spark"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInsatncing_Spark.hlsl"),
			VTXINSTANCE_PARTICLE_POINT::Elements, VTXINSTANCE_PARTICLE_POINT::iNumElements))))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Rect"),
		CRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("OBJ_Gui"),
		CGuiObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
		TEXT("Prototype_UILoadingScreen"),
		CUILoadingScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::CMainApp::Start_Level(LEVEL eStartLevel)
{
	if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel))))
		return E_FAIL;
	
	return S_OK;
}

unique_ptr<CMainApp> Client::CMainApp::Create()
{
	auto	pInstance = unique_ptr<CMainApp>(new CMainApp());
	if (FAILED(pInstance->Initialize()))
		return nullptr;

		return pInstance;
}
