#include "MainApp.h"
#include "ImGuiManager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
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
	
	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;
	
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	//A STAR
	{
	
		struct Text
		{
			_float X = 0;
			_float Z = 0;
			_float sum = 0;
			uint32_t Number = 0;
	
		};
	
		vector<Text> TextTileList;
		Text Pos0{}, Pos1{}, Pos2{}, Pos3{}, Pos4{}, Pos5{}, Pos6{}, Pos7{};
		Pos0.X = 1; Pos0.Z = 0; Pos0.Number = 1; Pos0.sum = sqrtf(Pos0.X * Pos0.X + Pos0.Z * Pos0.Z);
		Pos1.X = 0; Pos1.Z = 1; Pos1.Number = 2; Pos1.sum = sqrtf(Pos1.X * Pos1.X + Pos1.Z * Pos1.Z);
		Pos2.X = 1; Pos2.Z = 1; Pos2.Number = 3; Pos2.sum = sqrtf(Pos2.X * Pos2.X + Pos2.Z * Pos2.Z);
		Pos3.X = 2; Pos3.Z = 0; Pos3.Number = 4; Pos3.sum = sqrtf(Pos3.X * Pos3.X + Pos3.Z * Pos3.Z);
		Pos4.X = 1; Pos4.Z = 2; Pos4.Number = 5; Pos4.sum = sqrtf(Pos4.X * Pos4.X + Pos4.Z * Pos4.Z);
		Pos5.X = 2; Pos5.Z = 2; Pos5.Number = 6; Pos5.sum = sqrtf(Pos5.X * Pos5.X + Pos5.Z * Pos5.Z);
		Pos6.X = 2; Pos6.Z = 1; Pos6.Number = 7; Pos6.sum = sqrtf(Pos6.X * Pos6.X + Pos6.Z * Pos6.Z);
	
		TextTileList.push_back(Pos0);
		TextTileList.push_back(Pos1);
		TextTileList.push_back(Pos2);
		TextTileList.push_back(Pos3);
		TextTileList.push_back(Pos4);
		TextTileList.push_back(Pos5);
		TextTileList.push_back(Pos6);
	
		vector<ENGINE_ASTAR> OpenList, CloseList{};
		uint32_t Close_Number(0);
		ENGINE_ASTAR StartNode{}, LastNode{};
		LastNode.G = 0;
		LastNode.Pos = { 2,0,2 };
		StartNode.iParent_node = -1;
		StartNode.G = 0;
		StartNode.Pos = { 0,0,0 };
		//StartNode.H =   플레이어 - 보스
		//XMVector3
	
		//LastNode 플레이어 위치 담아놓기
		CloseList.push_back(StartNode); // 스타트 노드를 닫힌 노드에 우선 담기
		//////////////////////////////////////////////////////////////////////////
		while (true) // node가 도착 지점까지 전부 완료전까지
		{
			ENGINE_ASTAR TempNode{};
			///여기서 부터 for문인데///
			for (size_t i = 0; i < TextTileList.size(); ++i)
			{
				ENGINE_ASTAR Tmp{};
				//오픈 리스트가 비어있으면 첫번째 노드 위치 기준으로 pos값 먼저 추리기
				if (CloseList.size() <= 1)
				{
					// 스타트 노드 근처의 타일만 계산하기
					if ((TextTileList[i].X == CloseList[0].Pos.x + 1 && TextTileList[i].Z == CloseList[0].Pos.z) ||
						(TextTileList[i].X == CloseList[0].Pos.x - 1 && TextTileList[i].Z == CloseList[0].Pos.z) ||
						(TextTileList[i].X == CloseList[0].Pos.x && TextTileList[i].Z == CloseList[0].Pos.z + 1) ||
						(TextTileList[i].X == CloseList[0].Pos.x && TextTileList[i].Z == CloseList[0].Pos.z - 1))
					{
						Tmp.G = CloseList[0].G + 1;
						Tmp.Pos = { TextTileList[i].X, 0 , TextTileList[i].Z };
						Tmp.iNode_Nubmer = TextTileList[i].Number;
						Tmp.iParent_node = 0;
						Tmp.H = fabsf(LastNode.Pos.x - Tmp.Pos.x) + fabsf(LastNode.Pos.z - Tmp.Pos.z);
						Tmp.F = Tmp.H + Tmp.G;
						OpenList.push_back(Tmp);
					}
					//Tmp.Node_Nubmer // 타일 번호 넣기 중복 체크를 위함
					//타일 List의 pos 값넣기		
				}
				else
				{
					_bool bSame = false;
					//현재 열린 노드의 위치와 아니고 현재 위치 x + 1이면
					if ((TextTileList[i].X == CloseList[Close_Number].Pos.x + 1 && TextTileList[i].Z == CloseList[Close_Number].Pos.z) ||
						(TextTileList[i].X == CloseList[Close_Number].Pos.x - 1 && TextTileList[i].Z == CloseList[Close_Number].Pos.z) ||
						(TextTileList[i].X == CloseList[Close_Number].Pos.x && TextTileList[i].Z == CloseList[Close_Number].Pos.z + 1) ||
						(TextTileList[i].X == CloseList[Close_Number].Pos.x && TextTileList[i].Z == CloseList[Close_Number].Pos.z - 1))
					{
						Tmp.iParent_node = Close_Number;//이전 위치 담고
						Tmp.G = CloseList[Close_Number].G + 1;
						Tmp.H = fabsf(LastNode.Pos.x - Tmp.Pos.x) + fabsf(LastNode.Pos.z - Tmp.Pos.z);
						Tmp.F = Tmp.G + Tmp.H;
	
						Tmp.iNode_Nubmer = TextTileList[i].Number;
						Tmp.Pos = { TextTileList[i].X,0,TextTileList[i].Z };
						//Tmp.Node_Nubmer // 타일 번호 넣기 중복 체크를 위함
						//타일 List의 pos 값이 첫번쨰 노드 x + 1이면
						//현재 open list와 중복된 노드가있는지 확인
						for (auto iter = OpenList.begin(); iter != OpenList.end(); ++iter)
						{
							if (Tmp.iNode_Nubmer == (*iter).iNode_Nubmer)
							{
								bSame = true;
								//현재 OpenList의 Node Number와 겹치면
								if (Tmp.F >= (*iter).F)
								{
								}
								else if (Tmp.F < (*iter).F)
								{
									iter = OpenList.erase(iter);
									OpenList.push_back(Tmp);
									break;
								}
	
							}
						}
	
						if (!bSame)
							OpenList.push_back(Tmp);
					}
	
	
				}
			}
	
			//오픈 리스트가 비어있으면 스타트 기준으로
	
	
			uint32_t Tmp(0);
			_float Dst(5000);
	
			for (size_t i = 0; i < OpenList.size() - 1; ++i)
			{
				Tmp = min(OpenList[i].F, OpenList[i + 1].F);
				if (Dst > Tmp)
					Dst = Tmp;
			}
	
			for (auto iter = OpenList.begin(); iter != OpenList.end(); ++iter)
			{
				if (Dst == (*iter).F)
				{
					TempNode = *iter;
					iter = OpenList.erase(iter);
					break;
				}
			}
	
			CloseList.push_back(TempNode);
			++Close_Number;
	
			//마지막 노드의 위치랑 동일하면 나와라
			for (size_t i = 0; i < CloseList.size(); ++i)
			{
				if (CloseList[i].Pos.x == LastNode.Pos.x && CloseList[i].Pos.z == LastNode.Pos.z)
					return S_OK;
			}
		}
	
		//O 열린 목록, C 닫힌 목록
		//O 저장소는 최단 경로를 분석하기 위한 상태값들이 계속 생신됨
		//C 저장소는 처리가 완료된 노드를 담아 두기 위함
		//O와 C 기반으로 0번 노드에서 6번 노드가지의 최단 경로를 산출
		//1. 출발 노드를 닫힌 목록인 C 목록에 저장
		//2. 출발노드와 연결된 노드를 열린 목록 O 저장소에 추가
		//3. 추가할때 F G H Parent Node 값도 추가를ㄹ 해야됨Z
		//4. F = G + H, G는 시작 노드에서 해당 노드까지의 실제 소요 경비값.
		//   H는 휴리스틱 추정 값으로 해당 노드에서 최종 목적지까 소요 될 것이라고 추정 되는 값
		//5. Parent Node는 해당 노드에 도달하기 직전에 거치는 노드 번호
		//6. 출발점 1로 가정했을떄 2번 노드의 Parent Node는 0번 노드
		//  G 값은 1번 노드부터 2번 노드까지의 거리 비용 값 ex(x 2, z 3);
		//7. H값을 추정하기 위한 기준은 2번 노드에서 최종 목적지 6번 노드 까지의
		// 직선 거리로 ex H = 12,12
		//8.F는 14,15가됨 그렇게 노드를 담아두고 최소 값을 구하기
		//
	}

	return S_OK;
}

void Client::CMainApp::Update(float fTimeDelta)
{
	CGameInstance::Get().Update_Engine(fTimeDelta);

}

HRESULT Client::CMainApp::Render()
{
	const _float4 fColor = { 0.f,0.f,1.f,1.f };
	// RenderTarget 설정
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

	//if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC),
	//	TEXT("Component_InstanceTexter"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/InstancingTest.hlsl"),
	//		VERTEX_NOANIME::Elements, VERTEX_NOANIME::iNumElements))))
	//	return E_FAIL;

	if (FAILED(CGameInstance::Get().Add_Prototype(ETOUI(LEVEL::STATIC), TEXT("Prototype_Rect"),
		CRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
   //   	CGameInstance::Get().Add_Instancing_Shader(
	//static_pointer_cast<CShader>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), TEXT("Component_InstanceTexter"), nullptr)));
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
