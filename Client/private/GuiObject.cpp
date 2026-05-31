#include "GuiObject.h"
#include "GameInstance.h"
#include "NonModel.h"
#include "WorldObject.h"
#include "DecalObject.h"
#include "TriggerObject.h"
#include "DirectXCollision.h"
CGuiObject::CGuiObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CGuiObject::CGuiObject(const CGuiObject& Prototye) :CGameObject(Prototye)
{
}
CGuiObject::~CGuiObject()
{
};


HRESULT CGuiObject::Initialize_Prototype()
{
	auto iter = CGameInstance::Get().Get_TextureFileNameList();

	for (size_t i = 0; i < iter.size(); ++i)
	{
		ComPtr<ID3D11ShaderResourceView> pTexture;

		size_t iSize = MultiByteToWideChar(CP_UTF8, 0, iter[i].c_str(), ETOUI(iter[i].size()), NULL, 0);
		_wstring TriggerName(iSize, 0);
		MultiByteToWideChar(CP_UTF8, 0, iter[i].c_str(), ETOUI(iter[i].size()), TriggerName.data(), iSize);

		if (FAILED(CreateDDSTextureFromFile(m_pDevice.Get(), TriggerName.c_str(), nullptr, &pTexture)))
			return E_FAIL;

		m_GuiResources.push_back(pTexture);

	}
	//Layer_GameObject

	//m_strGameObjectPath = L"../../GasZone_Objects.json";
	//m_strTriggerPath = L"../../GasZone_Trigger.json";
	//m_strDecalpath = L"../../GasZone_Decal.json";
	//
	//m_strGameObject = "GasZone_Object";
	//m_strTrigger = "GasZone_Trigger";
	//m_strDecal = "GasZone_Decal";
	
	m_strGameObjectPath = L"../../Objects.json";
	m_strTriggerPath = L"../../Triggers.json";
	m_strDecalpath = L"../../Decal.json";
	
	m_strGameObject = "GameObjects";
	m_strTrigger = "Triggers";
	m_strDecal = "Decals";
	m_eLevel = LEVEL::GAMEPLAY;
	CGameInstance::Get().Add_Layer(ETOUI(m_eLevel), L"Layer_Temp");

	return S_OK;
}
HRESULT CGuiObject::Initialize(void* pArg)
{
	
	return S_OK;
}
void CGuiObject::Priority_Update(_float fTimeDelta)
{
	if (CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::F6))
		m_bModelCancle = !m_bModelCancle;



	if (m_bModelCancle)
		Navi_Creator();
	else
	{
		Save();
		Load_Data();
		Add_Decal_Texture();

	}
	
}
void CGuiObject::Update(_float fTimeDelta)
{
	

}
void CGuiObject::Late_Update(_float fTimeDelta)
{
	if (m_bModelCancle)
		return;
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	m_fMousePickXY.x = pt.x;
	m_fMousePickXY.y = pt.y;

	if (!m_bMouseCheck)
	{
		m_pObj = Picking_Object(m_LayerName);
		if (NULL_FALSE(m_pObj.lock()))
		{
			m_bMouseCheck = true;
			m_pObj.lock()->Set_bBoxColor(true);
			if (m_pObj.lock()->Get_MeshType() == MESH_TYPE::TRIGGER)
			{
				Click_Reset();
			}
		}
		ImGui::DebugDrawLineExtents();
	}
	Click_Reset();
	if (NULL_FALSE(m_pObj.lock()) && GetKeyState(VK_DELETE) & 0x8002)
	{
		m_pObj.lock()->Set_Dead();
		m_pObj.reset();
		m_bMouseCheck = false;
		
	}
}
HRESULT CGuiObject::Render()
{

	return S_OK;
}
HRESULT CGuiObject::Enable_GUI(string& name)
{
	if (m_bModelCancle)
		return S_OK;
	static	_bool	bCheck = true;
	static  _bool	test1 = true;
	static  _bool	test2 = true;
	static  _bool	test3 = true;
	static  _bool	test4 = true;
	static  _bool	test5 = true;
	static  _bool	test6 = true;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	if (!ImGui::Begin("Pratice", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
	{
		ImGui::End(); return S_OK;
	}//스크롤 접으면 밑에꺼 하지마라

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar(u8"탭슛", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"흠"))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(u8"메뉴")) // Menu
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu(u8"메뉴 오른쪽"))
				{
					ImGui::MenuItem("MainTest", NULL, bCheck);

					ImGui::SeparatorText("Mini apps");
					ImGui::MenuItem("abx", NULL);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (ImGui::CollapsingHeader("CollapsingHeader"))
			{
				ImGui::SeparatorText("SeparatorText");
				ImGui::BulletText("BulletText");
			}

			if (ImGui::CollapsingHeader("CollapsingHeader / Configuration"))
			{
				ImGuiIO& io = ImGui::GetIO();
				if (ImGui::TreeNode("TreeNode / Configuration#2"))
				{
					ImGui::SeparatorText(u8"타이틀 텍스트인듯");
					ImGui::CheckboxFlags(u8"플래그용 체크박스", &io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
					//방향키로 gui창 조정 가능하게 해주는거 좋은데?
					ImGui::SameLine(); ImGui::CheckboxFlags(u8"노 마우스", &io.ConfigFlags, ImGuiConfigFlags_NoMouse);
					if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)
					{
						if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f)
						{
							ImGui::SameLine();
							ImGui::Text(u8"스페이스");
						}
					}

					if (ImGui::IsKeyPressed(ImGuiKey_Space) || (io.ConfigFlags & ImGuiConfigFlags_NoKeyboard)) // 키보드 비활성화 일경우
						io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // ~를 해서 음수로 전환 0010 이면 1101로 
					//현재 플래그에 저장된것중 겹치는 비트를 끄기
					//비트 겹치지 않게 담아놓는거임 NoKey board는 64로 0100 0000 해당 비트자리만 사용중 6번째비트 사용중
					//ConfigFlags는 현재 총합 1041
					// NoKeyBoard ~ 1011 1111 음수로바꿔서 64번자리만 and 연산해서 빼버리기
					//근데 왜 총합은 1024가되냐
					// 1024 , 16 , 1 비트
					// 1011 1111
					// 0000 0001
					// 0000 0001 이래서?

					ImGui::SeparatorText(u8"윈도우");
					ImGui::Checkbox(u8"뭐지", &io.ConfigWindowsResizeFromEdges);// 창 크기 마우스로 조절 가능하게 하기
					ImGui::Checkbox(u8"타이틀 바로만 이동 가능", &io.ConfigWindowsMoveFromTitleBarOnly);

					ImGui::SeparatorText(u8"도킹슈");
					ImGui::CheckboxFlags(u8"도킹 활성화", &io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
					if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
					{
						ImGui::Indent(); //들여쓰기 시작
						ImGui::Checkbox(u8"도킹 탭바", &io.ConfigDockingAlwaysTabBar);//탭으로 이어 붙이기 가능;;
						//존나신기

						ImGui::Unindent();//들여쓰기 끝
					}


					ImGui::TreePop(); //트리 종료
					//ImGui::Spacing(); //세로 공간 살짞 띄우기
				}

				if (ImGui::TreeNode(u8"캡처"))
				{
					ImGui::LogButtons();//뭐지

					if (ImGui::Button(u8"문자열 복사가능"))
					{
						ImGui::LogToClipboard();
						ImGui::LogText(u8"복사");
						ImGui::LogFinish();
					}

					ImGui::TreePop();
				}
			}


			if (ImGui::CollapsingHeader(u8"윈도우 옵션"))
			{
				if (ImGui::BeginTable(u8"컬럼 칸수 지정가능", 3)) // 3칸씩 해주네
				{
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트", &test1);
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트1", &test2);
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트2", &test3);
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트3", &test4);
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트4", &test5);
					ImGui::TableNextColumn(); ImGui::Checkbox(u8"테스트5", &test6);
					ImGui::EndTable();
				}
			}

			if (ImGui::CollapsingHeader(u8"위젯"))
			{
				if (ImGui::TreeNode(u8"베이직"))
				{
					static int clicked = 0;
					if (ImGui::Button(u8"버튼"))
						++clicked;
					if (clicked & 1)
					{
						ImGui::SameLine();
						ImGui::Text(u8"홀수");
					}
					static bool check = true;
					ImGui::Checkbox(u8"체크 박스", &check);
					static int e = 0;
					ImGui::RadioButton(u8"라디오 a", &e, 0); ImGui::SameLine();
					ImGui::RadioButton(u8"라디오 b", &e, 1); ImGui::SameLine();
					ImGui::RadioButton(u8"라디오 c", &e, 2);

					ImGui::TextLinkOpenURL(u8"흠", "https://www.naver.com");


					ImGui::SeparatorText(u8"버튼 색상 관련");
					for (int i = 0; i < 7; ++i)
					{
						if (i > 0)
							ImGui::SameLine();
						ImGui::PushID(i);
						//버튼 기본, 마우스 올렷을 때, 눌렀을때
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.2f, 0.6f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.6f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.6f));
						ImGui::Button("Clock");
						ImGui::PopStyleColor(3);
						ImGui::PopID();
					}

					ImGui::SeparatorText(u8"좌 우 버튼");
					static int counter = 0;
					float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
					ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);// 버튼 꾹 누를수있게 해주는거
					if (ImGui::ArrowButton("#left", ImGuiDir_Left)) { counter--; }
					ImGui::SameLine(0.0f, spacing);
					if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
					ImGui::PopItemFlag(); ImGui::SameLine();
					ImGui::Text("%d", counter);

					ImGui::SeparatorText(u8"툴팁");
					ImGui::Button(u8"툴팁버튼");
					ImGui::SetItemTooltip(u8"툴팁임");

					ImGui::SeparatorText(u8"인풋");
					ImGui::LabelText(u8"라벨", u8"값");

					static char str0[128] = u8"헬로";
					ImGui::InputText(u8"텍스트 입력", str0, IM_COUNTOF(str0));

					static char str1[128] = "";
					ImGui::InputTextWithHint(u8"힌트", u8"여기에 입력", str1, IM_COUNTOF(str1));
					static int i0 = 123;
					ImGui::InputInt(u8"정수 입력", &i0);

					static float f0 = 123;
					ImGui::InputFloat(u8"실수 입력", &f0, 0.01f, 1.0f, "%.3f");

					static float vec4a[4] = { 0.10f,0.20f,0.30f,0.44f };
					ImGui::InputFloat3(u8"실수 3개", vec4a);

					ImGui::SeparatorText(u8"드래그");
					static int i1 = 50, i2 = 42, i3 = 128;
					ImGui::DragInt(u8"드래그 정수", &i1, 1);
					ImGui::DragInt(u8"드래그 정수 0~100", &i2, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
					//값이 0 ~ 100 사이에 강제 하도록 설정하는 falgs enum값임

					static float f1 = 1.00f, f2 = 0.0067f;
					ImGui::DragFloat(u8"드래그 실수", &f1, 0.005f);
					ImGui::DragFloat(u8"작은 실수", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");


					ImGui::SeparatorText(u8"슬라이더");
					static int i01 = 0;
					ImGui::SliderInt(u8"인트 슬라이더", &i01, -1, 3);
					static float af = 0.123f, a1f = 0.f;
					ImGui::SliderFloat(u8"실수 슬라이더", &af, 0.f, 1.f, "ratio = %3.f");
					ImGui::SliderAngle(u8"앵글 슬라이더", &a1f);

					enum Element { Ele_Fire, Ele_Earth, Ele_Air, Ele_Wat, Ele_Cnt };
					static int elem = Ele_Fire;
					const char* elem_names[Ele_Cnt] = { "Fire","Earth","Air","Water" };
					const char* elem_name = (elem >= 0 && Ele_Cnt) ? elem_names[elem] : "Unknown";
					ImGui::SliderInt(u8"이넘", &elem, 0, Ele_Cnt - 1, elem_name);


					ImGui::SeparatorText(u8"피커스");
					const char* items[] = { "AAA","BBB","CCC","DDDD","EEEE","FFFF" };
					static int item_current = 0;
					ImGui::Combo("Combo", &item_current, items, IM_COUNTOF(items));


					static int item_currentA = 1;
					static bool Chk = true;
					const char* itemsB[] = { "Apple","Banana","Cherry","Kiwi" };
					ImGui::Checkbox("Btn", &Chk);
					if (Chk)
						ImGui::ListBox("listBox", &item_currentA, itemsB, IM_COUNTOF(itemsB), 4);

					ImGui::TreePop();
				}
				if (ImGui::TreeNode(u8"콤보스"))
				{
					ImGui::SeparatorText(u8"첫콤");
					static ImGuiComboFlags flags = 0;
					ImGui::CheckboxFlags(u8"콤보팝업 왼쪽정렬", &flags, ImGuiComboFlags_PopupAlignLeft);
					//콤보 팝업을 기본적으로 왼쪽으로 정렬
					if (ImGui::CheckboxFlags(u8"오른쪽 화살표 제거", &flags, ImGuiComboFlags_NoArrowButton))
						flags &= ~ImGuiComboFlags_NoPreview;
					//화살표 버튼만 보이게
					if (ImGui::CheckboxFlags(u8"화살표 켜져있으면 충돌 옵션 제거", &flags, ImGuiComboFlags_NoPreview))
						flags &= ~(ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_WidthFitPreview);
					if (ImGui::CheckboxFlags(u8"내용 길이에 자동 정렬", &flags, ImGuiComboFlags_WidthFitPreview))
						flags &= ~ImGuiComboFlags_NoPreview;
					//내용길이 자동정렬시 미리보기 제거 해주는 옵션 다시 활성화

					if (ImGui::CheckboxFlags(u8"최대 4개까지만 보이게", &flags, ImGuiComboFlags_HeightSmall))
						flags &= ~(ImGuiComboFlags_HeightMask_ & ~ImGuiComboFlags_HeightSmall);
					//small을 제외한 나머지 옵션을 다 꺼라
					if (ImGui::CheckboxFlags(u8"최대8개", &flags, ImGuiComboFlags_HeightRegular))
						flags &= ~(ImGuiComboFlags_HeightMask_ & ~ImGuiComboFlags_HeightRegular);
					if (ImGui::CheckboxFlags(u8"최대20개", &flags, ImGuiComboFlags_HeightLargest))
						flags &= ~(ImGuiComboFlags_HeightMask_ & ~ImGuiComboFlags_HeightLargest);


					const char* items[] = { "AAA","BBB","CCC","DDD","EEE","FFF","GGG","HHH","III","JJJ","kkk","LLL" };
					static int item_selected_idx = 0;

					const char* combo_preview_value = items[item_selected_idx];
					if (ImGui::BeginCombo(u8"콤보1", combo_preview_value, flags))
					{
						for (int i = 0; i < IM_COUNTOF(items); ++i)
						{
							const bool is_selected = (item_selected_idx == i);
							if (ImGui::Selectable(items[i], is_selected)) // 선택한 문자열
								item_selected_idx = i;

							if (is_selected)
								ImGui::SetItemDefaultFocus(); //선택된 아이템을 포커스하라
						}
						ImGui::EndCombo();
					}

					if (ImGui::BeginCombo(u8"콤보 필터", combo_preview_value, flags))
					{
						static ImGuiTextFilter filter;
						if (ImGui::IsWindowAppearing())
						{
							//창이 열리면 거기에 입력 가능하게 포커스, 기존 입력 제거
							ImGui::SetKeyboardFocusHere();
							filter.Clear();
						}

						ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
						//ctrl + f 로 포커스 맞추기
						filter.Draw("##필터", -FLT_MIN); //가로로 꽉찬입력창

						for (int i = 0; i < IM_COUNTOF(items); ++i)
						{
							const bool is_selected = (item_selected_idx == i);
							if (filter.PassFilter(items[i]))
								if (ImGui::Selectable(items[i], is_selected))
									item_selected_idx = i;
						}

						ImGui::EndCombo();
					}

					static int item_current_2 = 0;
					ImGui::Combo(u8"한줄", &item_current_2, "aaa\0bbbb0\cccc\0ddd\0eee\0\0");

					static int item_current3 = -1;
					ImGui::Combo(u8"배열", &item_current3, items, IM_COUNTOF(items));
					//근데 저거 두개는 비추라는데;;

					static int item_current_4 = 0;
					ImGui::Combo(u8"함수", &item_current_4, [](void* data, int n) { return ((const char**)data)[n]; }, items, IM_COUNTOF(items));
					//이거도좀 구린듯
					ImGui::TreePop();


				}

				if (ImGui::TreeNode(u8"리스트"))
				{
					ImGui::PushItemWidth(ImGui::CalcTextSize("                    ").x);
					const char* items[4][4] = { {"AAAA", "BBBB", "CCCC", "DDDD"},{"AA2","BB2","CC2","DD2"},{"AA3","BB3","CC3","DD3"},{"AA4","BB4","CC4","DD4"} };
					static int item = -1;
					ImGui::Combo("Combo", &item, *(&(*items)), IM_COUNTOF(items) * 4);

					static int selection[4] = { 0,1,2,3 };
					for (int i = 0; i < 4; ++i)
					{
						if (i > 0) ImGui::SameLine();
						ImGui::PushID(i);
						ImGui::ListBox("", &selection[i], items[i], IM_COUNTOF(items[i]));
						ImGui::PopID();
					}

					ImGui::PopItemWidth();
					ImGui::TreePop();
				}

				if (ImGui::TreeNode(u8"인풋 포커스"))
				{
					bool focus_1 = ImGui::Button(u8"1번"); ImGui::SameLine();
					bool focus_2 = ImGui::Button(u8"2번"); ImGui::SameLine();
					bool focus_3 = ImGui::Button(u8"3번");
					int has_focus = 0;
					static char buf[128] = u8"클릭하라";

					if (focus_1) ImGui::SetKeyboardFocusHere();
					ImGui::InputText("1", buf, IM_COUNTOF(buf));
					if (ImGui::IsItemActive()) has_focus = 1;


					if (focus_2) ImGui::SetKeyboardFocusHere();
					ImGui::InputText("2", buf, IM_COUNTOF(buf));
					if (ImGui::IsItemActive()) has_focus = 2;

					ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
					if (focus_3) ImGui::SetKeyboardFocusHere();
					ImGui::InputText(u8"3 탭 스킵?", buf, IM_COUNTOF(buf));
					if (ImGui::IsItemActive()) has_focus = 3;
					ImGui::PopItemFlag();

					if (has_focus)
						ImGui::Text(u8"아이템 초점 %d", has_focus);
					else
						ImGui::Text(u8"흠");


					static _float f3[3] = { 0.f,0.f,0.f };
					int focus_ahead = -1;

					if (ImGui::Button("X")) { focus_ahead = 0; }ImGui::SameLine();
					if (ImGui::Button("Y")) { focus_ahead = 1; }ImGui::SameLine();
					if (ImGui::Button("Z")) { focus_ahead = 2; }
					if (focus_ahead != -1) ImGui::SetKeyboardFocusHere(focus_ahead);
					ImGui::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

					ImGui::TreePop();
				}

			}

			if (ImGui::CollapsingHeader(u8"테이블 컬럼"))
			{
				const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
				const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

				ImGui::PushID(u8"테이블");

				int open_action = -1;
				if (ImGui::Button(u8"전부 펼치기"))
					open_action = 1;
				ImGui::SameLine();
				if (ImGui::Button(u8"전부 접기"))
					open_action = 0;
				ImGui::SameLine();


				static bool disable_indent = false;
				ImGui::Checkbox(u8"트리 접기", &disable_indent);
				ImGui::Separator();
				if (disable_indent)
					ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.f);

				if (open_action != -1)
					ImGui::SetNextItemOpen(open_action != 0);

				if (ImGui::TreeNode(u8"베이직"))
				{
					if (ImGui::BeginTable(u8"테이블3 칸", 3))
					{
						for (int row = 0; row < 4; ++row)
						{
							ImGui::TableNextRow();
							for (int column = 0; column < 3; ++column)
							{
								ImGui::TableSetColumnIndex(column);
								ImGui::Text(u8"로우 %d, 컬럼 %d", row, column);
							}
						}
						ImGui::EndTable();
					}
					ImGui::TreePop();
				}

				ImGui::PopID();

				if (disable_indent)
					ImGui::PopStyleVar();


				if (open_action != -1)
					ImGui::SetNextItemOpen(open_action != 0);
				if (ImGui::TreeNode(u8"백그라운드"))
				{
					enum ContentsType { CT_TEXT, CT_FillButton };
					static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
					static bool display_headers = false;
					static int contents_type = CT_TEXT;

					ImGui::CheckboxFlags(u8"배경색", &flags, ImGuiTableFlags_RowBg);
					ImGui::CheckboxFlags(u8"테이블 경계선", &flags, ImGuiTableFlags_Borders);
					ImGui::Indent(); //오른쪽 들여쓰기

					ImGui::CheckboxFlags(u8"가로줄만 표시", &flags, ImGuiTableFlags_BordersH);
					ImGui::Indent();
					ImGui::CheckboxFlags(u8"테이블 위 아래 가로줄만", &flags, ImGuiTableFlags_BordersOuterH);
					ImGui::CheckboxFlags(u8"행사이 가로 구분줄만 위 아래는 없음", &flags, ImGuiTableFlags_BordersInnerH);
					ImGui::Unindent();

					ImGui::CheckboxFlags(u8"세로줄만 표시", &flags, ImGuiTableFlags_BordersV);
					ImGui::Indent();
					ImGui::CheckboxFlags(u8"세로 양쪽만", &flags, ImGuiTableFlags_BordersOuterV);
					ImGui::CheckboxFlags(u8"세로 안쪽만", &flags, ImGuiTableFlags_BordersInnerV);
					ImGui::Unindent();

					ImGui::CheckboxFlags(u8"가로세로 바깥 전체", &flags, ImGuiTableFlags_BordersOuter);
					ImGui::CheckboxFlags(u8"가로세로 내부 전체", &flags, ImGuiTableFlags_BordersInner);
					ImGui::Unindent();


					ImGui::AlignTextToFramePadding(); ImGui::Text(u8"셀 콘텐츠"); //세로정렬
					ImGui::SameLine(); ImGui::RadioButton(u8"문자", &contents_type, CT_TEXT);
					ImGui::SameLine(); ImGui::RadioButton(u8"필버튼", &contents_type, CT_FillButton);
					ImGui::Checkbox(u8"디스플레이 헤더", &display_headers);//테이블 위에 헤더 보일지
					ImGui::CheckboxFlags(u8"본문에서 세로줄 제거", &flags, ImGuiTableFlags_NoBordersInBody);


					ImGui::Text(u8"헤더 보이기 숨기기");
					if (ImGui::BeginTable(u8"진짜 테이블 3칸 플래그 정보도 넘기기", 3, flags))
					{
						if (display_headers)
						{
							ImGui::TableSetupColumn("One"); //헤더 이름 지정
							ImGui::TableSetupColumn("Two");
							ImGui::TableSetupColumn("Three");
							ImGui::TableHeadersRow(); //헤더 생성
						}

						for (int row = 0; row < 5; ++row)
						{
							ImGui::TableNextRow();
							for (int column = 0; column < 3; ++column)
							{
								ImGui::TableSetColumnIndex(column);
								char buff[32];
								sprintf_s(buff, u8"안녕 %d,%d", column, row);
								if (contents_type == CT_TEXT)
									ImGui::TextUnformatted(buff);
								else if (contents_type == CT_FillButton)
									ImGui::Button(buff, ImVec2(-FLT_MIN, 0.0f));
							}
						}

						ImGui::EndTable();
					}


					ImGui::TreePop();
				}

				if (ImGui::TreeNode(u8"가로 스크롤링"))
				{
					static ImGuiTableFlags flags = ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
					static int freeze_cols = 4;
					static int freeze_rows = 1;

					ImGui::CheckboxFlags(u8"컬럼 너비 마우스로 조정 가능", &flags, ImGuiTableFlags_Resizable);
					ImGui::CheckboxFlags(u8"가로 스크롤 활성화", &flags, ImGuiTableFlags_ScrollX);
					ImGui::CheckboxFlags(u8"세로 스크롤 활성화", &flags, ImGuiTableFlags_ScrollY);
					ImGui::SetNextItemWidth(ImGui::GetFrameHeight()); //정사각형 느낌으로 작게 만듬
					ImGui::DragInt(u8"왼쪽에 고정할수 4개로해볼까", &freeze_cols, 0.2f, 0, 4, NULL, ImGuiSliderFlags_NoInput);
					ImGui::SetNextItemWidth(ImGui::GetFrameHeight());
					ImGui::DragInt(u8"세로는 5개정도로", &freeze_rows, 0.2f, 0., 5, NULL, ImGuiSliderFlags_NoInput);

					ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
					if (ImGui::BeginTable(u8"테이블 스크롤x", 7, flags, outer_size))
					{
						ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
						ImGui::TableSetupColumn(u8"컬럼못끄게하기", ImGuiTableColumnFlags_NoHide);
						ImGui::TableSetupColumn(u8"가");
						ImGui::TableSetupColumn(u8"나");
						ImGui::TableSetupColumn(u8"다");
						ImGui::TableSetupColumn(u8"라");
						ImGui::TableSetupColumn(u8"마");
						ImGui::TableHeadersRow();



						for (int row = 0; row < 20; ++row)
						{
							ImGui::TableNextRow();
							for (int column = 0; column < 7; ++column)
							{

								if (!ImGui::TableSetColumnIndex(column) && column > 0)
									continue;
								if (column == 0)
									ImGui::Text(u8"라인 %d", row);
								else
									ImGui::Text(u8"안녕 %d,%d", column, row);
							}
						}
						ImGui::EndTable();

					}
					ImGui::TreePop();
				}

			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"모델 로더"))
		{
			Select_Model();
			
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem(u8"모델 연결"))
		{
			Connect_Model();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"라이트"))
		{
			Light_Setting();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"다중 복사"))
		{
			Multy_Copy();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"레이어 이동"))
		{
			Move_Layer();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}


	ImGui::End();

	if (!ImGui::Begin(u8"기즈모", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
	{
		ImGui::End(); return S_OK;
	}
	if (ImGui::BeginTabBar(u8"기능", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"기즈모"))
		{
			if (NULL_TRUE(m_pObj.lock()))
			{
				ImGui::Text(u8"선택된 오브젝트가 없음");
	
			}
			else
				ImGui_Gizmo();
	
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	//첫번째 오브젝트가 가진 타겟의 중심과 이어주는 선 만들기
	//타겟 정보는 트리거 컴포넌트가 들고 있는데그럼 거기에 매개변수 인자로 넘겨버리면 되겠네
	//라인은 해당 방향으로 거리를 크기로 계산해서 더하면 될듯?

	ImGui::End();

	return S_OK;
}

void CGuiObject::Select_Model()
{
	string path = "";
	static const char*	cSelect[] = { "Anmiation","Default" };
	static int32_t		iModelButton(1);
	static int32_t		iMode(0);
	static int32_t		iLayerMode(0);
	static string		strTriggerName;
	static int32_t		iOtherTriggerMode(1);
	static WORLD_EVENT  eEvent{ WORLD_EVENT::END };
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar(u8"탭슛", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"모델"))
		{
			ImGui::RadioButton(u8"모델용",       &iModelButton, ETOUI(MESH_TYPE::NONANIME)); ImGui::SameLine();
			ImGui::RadioButton(u8"트리거용",	 &iModelButton, ETOUI(MESH_TYPE::TRIGGER));
			
			ImGui::RadioButton(u8"오브젝트 레이어", &iLayerMode, 0); ImGui::SameLine();
			ImGui::RadioButton(u8"트리거 레이어  ", &iLayerMode, 1); ImGui::SameLine();
			ImGui::RadioButton(u8"데칼 레이어  ", &iLayerMode, 2); ImGui::SameLine();
			ImGui::RadioButton(u8"임시 레이어 ", &iLayerMode, 3);

			ImGui::RadioButton(u8"추가 트리거 활성화",   &iOtherTriggerMode, 1); ImGui::SameLine();
			ImGui::RadioButton(u8"추가 트리거 비활성화", &iOtherTriggerMode, 0);

			if (iLayerMode == 0)
				m_LayerName = L"Layer_WorldObject";
			if (iLayerMode == 1)
				m_LayerName = L"Layer_TriggerObject";
			if (iLayerMode == 2)
				m_LayerName = L"Layer_Decal";
			if (iLayerMode == 3)
			{
				m_LayerName = L"Layer_Temp";
				ImGui::Text(u8"임시 저장소에는 추가 불가");
				ImGui::EndTabItem();
				ImGui::EndTabBar();
				return;
			}

			if (iLayerMode == 2)
			{
				ImGui::EndTabItem();
				ImGui::EndTabBar();
				return;
			}//233 230
			GAMEOBJECT_DESC desc{};
			if (iModelButton == ETOUI(MESH_TYPE::TRIGGER))
			{
				ImGui::Text(u8"트리거 옵션");
				const char* items[] = { "OBJ_Door","OBJ_Lever","OBJ_RollupDoor","OBJ_GreenElectric","OBJ_Battery","OBJ_BatteryCase","OBJ_BlueElectric","OBJ_ElectricPole","OBJ_PoleHead","OBJ_ElectricPannel","OBJ_LowerFlip","OBJ_LowerFlip_Flip" 
										, "OBJ_Generator"};
				const char* Rotitems[] = { "X","Y","Z" };
				const char* WorldEventItem[] = { "DOOR","GENERATOR","TEACHER_SPAWN" ,"BATTERY","ROLLUP_DOOR","TEACHER_DEAD","BOSS_TP","END"};
				static int item_WorldEvent = IM_COUNTOF(WorldEventItem) - 1;
				static int item_selected_idx = 0;
				static int imte_rotselected_idx = 0;
				const char* combo_preview_value = items[item_selected_idx];
				const char* combo_preview_Rotvalue = Rotitems[imte_rotselected_idx];
				const char* combo_preview_Eventvalue = WorldEventItem[item_WorldEvent];
				if (ImGui::BeginCombo(u8"옵션", combo_preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
				{
					for (int i = 0; i < IM_COUNTOF(items); ++i)
					{
						const bool is_selected = (item_selected_idx == i);
						if (ImGui::Selectable(items[i], is_selected)) // 선택한 문자열
							item_selected_idx = i;

						if (is_selected)
							ImGui::SetItemDefaultFocus();
						//선택된 아이템을 포커스하라
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine(150);
				if (ImGui::BeginCombo(u8"월드 이벤트", combo_preview_Eventvalue, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
				{
					for (int i = IM_COUNTOF(WorldEventItem) - 1; i >= 0; --i)
					{
						const bool is_selected = (item_WorldEvent == i);
						if (ImGui::Selectable(WorldEventItem[i], is_selected)) // 선택한 문자열
							item_WorldEvent = i;

						if (is_selected)
							ImGui::SetItemDefaultFocus();
						//선택된 아이템을 포커스하라
					}
					ImGui::EndCombo();
				}

				eEvent = static_cast<WORLD_EVENT>(item_WorldEvent);
				ImGui::SameLine(300);
				if (ImGui::BeginCombo(u8"회전방향", combo_preview_Rotvalue, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
				{
					for (int j = 0; j < IM_COUNTOF(Rotitems); ++j)
					{
						const bool is_selected = (imte_rotselected_idx == j);
						if (ImGui::Selectable(Rotitems[j], is_selected)) // 선택한 문자열
							imte_rotselected_idx = j;

						if (is_selected)
							ImGui::SetItemDefaultFocus();
						//선택된 아이템을 포커스하라
					}
					ImGui::EndCombo();
				}
				strTriggerName = items[item_selected_idx];
				desc.eRot = static_cast<TRIGGER_ROT>(imte_rotselected_idx);
				static _float fTimeTick{}, fTimeFrame{}, fRotSpeed{};
				ImGui::InputFloat(u8"최대 각도", &fTimeTick, ImGuiComboFlags_WidthFitPreview);
				ImGui::InputFloat(u8"최대 시간", &fTimeFrame, ImGuiComboFlags_WidthFitPreview); 
				ImGui::InputFloat(u8"회전 속도", &fRotSpeed, ImGuiComboFlags_WidthFitPreview);

				desc.fArrrowRotation = fRotSpeed;
				desc.fFrameTickTime = fTimeTick;
				desc.fMaxFrameTime = fTimeFrame;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1,0,0,1 });
			ImGui::Text(u8"현재 선택된 옵션 : "); ImGui::SameLine(); 
			ImGui::Text(strTriggerName.c_str());
			ImGui::PopStyleColor();

			ImGui::RadioButton(u8"모델 최적화", &iMode, 0); ImGui::SameLine();
			ImGui::RadioButton(u8"단일 모델",   &iMode, 1);

			ImGui::Text(u8"랜더 콜"); ImGui::SameLine(), ImGui::Text(to_string(CGameInstance::Get().Get_RanderCall()).c_str());
			if (ImGui::Button(u8"콜리전 켜기"))
				CGameInstance::Get().Set_Collision(true);		
			ImGui::SameLine(); 
			if (ImGui::Button(u8"콜리전 끄기")) 
				CGameInstance::Get().Set_Collision(false);
			ImGui::Text(u8"현재 오브젝트 번호 :"); ImGui::SameLine();
			ImGui::Text(u8"대상 오브젝트 번호 :"); ImGui::SameLine();


			auto pObj = m_pObj.lock();
			if (!m_bCopy && NULL_FALSE(pObj) && GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('C') & 0x8000)
			{	
				switch (pObj->Get_MeshType())
				{
				case MESH_TYPE::NONANIME:
					break;
				case MESH_TYPE::TRIGGER:
					m_CopyTriggerName = static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerInfo().strTriggerName;
					break;
				}
				 m_bCopy = true;
				 XMStoreFloat4x4(&m_CopyWorld, pObj->Get_Transform().lock()->Get_World());
			}
			
			if (m_bCopy)
			{
				ImGui::Text(u8"현재 복사된 오브젝트 : "); ImGui::SameLine(); ImGui::Text(pObj->Get_PathName().c_str());
			}
			else ImGui::Text(u8"복사할 오브젝트가 선택되지 않음");

			if (m_bCopy && NULL_FALSE(pObj) && GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('V') & 0x8000)
			{
				m_CopyWorld._41 = m_CopyWorld._41 + 3.f; // 카메라 보다 조금 앞에 생성하기
				desc.FileName	 = pObj->Get_PathName();			//매쉬 경로 복사		
				desc.matWorld = m_CopyWorld;
				desc.iModeNumber = iMode;				//뭐였드라;
				desc.bCopy = true;						//복사 한다는 뜻
				desc.eType = pObj->Get_MeshType();			// 매쉬 타입
				desc.strTriggerName = m_CopyTriggerName; //트리거 밸류 저장
				
				switch (pObj->Get_MeshType())
				{
				case MESH_TYPE::NONANIME:
					CGameInstance::Get().Add_GameObject_toLayer(ETOUI(m_eLevel), L"OBJ_WorldObject", ETOUI(m_eLevel), L"Layer_WorldObject", &desc);
					break;
				case MESH_TYPE::TRIGGER:
					desc.bTrigger = static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerInfo().bOtherTrigger;
					CGameInstance::Get().Add_GameObject_toLayer(ETOUI(m_eLevel), L"OBJ_Trigger", ETOUI(m_eLevel), L"Layer_TriggerObject", &desc);
					break;
				}
				
				m_bMouseCheck = false;
				
				pObj->Set_bBoxColor(false);
				m_pObj.reset();
				m_bCopy = false;
				m_CopyTriggerName = "";
				m_CopyWorld = {};
			}


			if (ImGui::TreeNode(u8"선택"))
			{
				static _bool h_borders = true;
				static _bool v_borders = true;
				static int32_t columns_count = 4;
				static ImGuiTextFilter filter;

				auto  pPath = CGameInstance::Get().Get_FileNameList();

				const  int32_t lines_count = pPath.size();
				ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
				if (columns_count < 2)
					columns_count = 2;
				ImGui::SameLine();
				ImGui::Checkbox(u8"가로줄", &h_borders);
				ImGui::SameLine();
				ImGui::Checkbox(u8"세로줄", &v_borders);
				ImGui::Columns(columns_count, NULL, v_borders);
				filter.Draw();
				for (int i = 0; i < lines_count; ++i)
				{
					if (h_borders && ImGui::GetColumnIndex() == 0)
						ImGui::Separator();

					ImGui::PushID(i);
					if (filter.PassFilter(pPath[i].c_str()))
					{
						ImGui::Text(pPath[i].c_str(), ImGui::GetColumnWidth());
						if (ImGui::Button(u8"선택"))
						{
							_vector vRight = { 1,0,0,0 }, vUp = { 0, 1, 0 ,0 }, vLook = {}, vPos = {};

							desc.FileName = CGameInstance::Get().Find_Path(pPath[i]);
							desc.iModeNumber = iMode;
							desc.bFrontCamera = false;
							desc.eType = static_cast<MESH_TYPE>(iModelButton);
							_float4x4 matWorld = {};
							//뭐드라 이거

							XMStoreFloat4x4(&matWorld, XMLoadFloat4x4(CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW)));
							memcpy(&matWorld.m[0], &vRight, sizeof(_vector));
							memcpy(&matWorld.m[1], &vUp, sizeof(_vector));

							memcpy(&vLook, &matWorld.m[2], sizeof(_vector));
							memcpy(&vPos, &matWorld.m[3], sizeof(_vector));

							XMVector3Normalize(vLook);
							vPos += vLook * 10.f;
							vLook = { 0,0,1,0 };
							memcpy(&desc.matWorld.m[0], &vRight, sizeof(_vector));
							memcpy(&desc.matWorld.m[1], &vUp,	 sizeof(_vector));
							memcpy(&desc.matWorld.m[2], &vLook,	 sizeof(_vector));
							memcpy(&desc.matWorld.m[3], &vPos,	 sizeof(_vector));
							
							switch (desc.eType)
							{
							case MESH_TYPE::NONANIME:
								CGameInstance::Get().Add_GameObject_toLayer(ETOUI(m_eLevel), L"OBJ_WorldObject",
									ETOUI(m_eLevel), L"Layer_WorldObject", &desc);
								break;
							case MESH_TYPE::TRIGGER:
								desc.strTriggerName = strTriggerName;
								desc.bTrigger       = iOtherTriggerMode;
								desc.eWroldEvent = eEvent;
								CGameInstance::Get().Add_GameObject_toLayer(ETOUI(m_eLevel), L"OBJ_Trigger",
									ETOUI(m_eLevel), L"Layer_TriggerObject", &desc);
								break;
							}
							
						}
					}

					ImGui::PopID();
					ImGui::NextColumn();
				}

				ImGui::Columns(1);
				if (h_borders)
					ImGui::Separator();

				ImGui::TreePop();

			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

	}

}

void CGuiObject::ImGui_Gizmo()
{
	//테스트
	
	auto pObj = m_pObj.lock();
	XMMATRIX matRota = XMMatrixIdentity();
	XMMATRIX matWorld = pObj->Get_Transform().lock()->Get_World();

	ImGui::Text(u8"오브젝트 이름 : "); ImGui::SameLine(); ImGui::Text(pObj->Get_PathName().c_str());
	
	_bool enable = true;
	ImGuizmo::IsOver();
	ImGuizmo::IsUsing();

	ImGuizmo::Enable(enable);
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton(u8"이동", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"회전", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton(u8"크기", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	static _float  matTrans[3] = {}, matRot[3] = {}, matScale[3] = {};

	ImGuizmo::DecomposeMatrixToComponents((float*)&(matWorld),matTrans, matRot, matScale);
	ImGui::InputFloat3(u8"ㄹㅇ이동", matTrans);
	ImGui::InputFloat3(u8"ㄹㅇ회전", matRot);
	ImGui::InputFloat3(u8"ㄹㅇ스케일", matScale);

	ImGuizmo::RecomposeMatrixFromComponents(matTrans, matRot, matScale, (float*)&matWorld);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	static _bool	useSnap(false);
	if (ImGui::IsKeyPressed(ImGuiKey_S))
		useSnap = !useSnap;
	ImGui::Checkbox(u8"##useSnap", &useSnap);
	ImGui::SameLine();
	_float* f3 = nullptr ;
	switch(mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		f3 = matTrans;
		ImGui::InputFloat3(u8"스냅 이동", f3);
		break;
	case ImGuizmo::ROTATE:
		f3 = matRot;
		ImGui::InputFloat3(u8"스냅 회전", f3);
		break;
	case ImGuizmo::SCALE:
		f3 = matScale;
		ImGui::InputFloat3(u8"스냅 크기", f3);
		break;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	XMMATRIX mView       = XMLoadFloat4x4(CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	XMMATRIX mProjection = XMLoadFloat4x4(CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	ImGuizmo::Manipulate(
		(float*)&mView,
		(float*)&mProjection,
		mCurrentGizmoOperation,
		ImGuizmo::WORLD,
		(float*)&matWorld, f3
	);

	pObj->Get_Transform().lock()->Set_State(STATE::RIGHT, matWorld.r[0]);
	pObj->Get_Transform().lock()->Set_State(STATE::UP, matWorld.r[1]);
	pObj->Get_Transform().lock()->Set_State(STATE::LOOK, matWorld.r[2]);
	pObj->Get_Transform().lock()->Set_State(STATE::POS, matWorld.r[3]);
	
}

weak_ptr<CGameObject> CGuiObject::Picking_Object(const _wstring& LayerName)
{

	Engine::COLLISION_DESC desc;
	desc.matProj = *CGameInstance::Get().Get_Transform(D3DTS::PROJ);
	desc.matView = *CGameInstance::Get().Get_Transform(D3DTS::VIEW);
	desc.fDir = m_fMouseLocalDir;
	desc.fPos = m_fMosueLocalPos;
	desc.fMouse = m_fMousePickXY;
	weak_ptr<CGameObject> pObj = {};

	pObj = CGameInstance::Get().Check_Ray(ETOUI(m_eLevel), LayerName, desc);
	
	m_fMouseLocalDir = desc.fDir ;
	m_fMosueLocalPos = desc.fPos ;
	
	if (NULL_TRUE(pObj.lock()))
		return {};
		

	return pObj;

}
void CGuiObject::Light_Setting()
{
	auto pObj = m_pObj.lock();
	if (NULL_TRUE(pObj))
		return;


	if (ImGui::TreeNode(u8"라이트 슛"))
	{
		auto LightMtrl = CGameInstance::Get().Find_LightMtrl(pObj->Get_PathName()).lock();

		if (NULL_TRUE(LightMtrl))
		{
			ImGui::TreePop();
			return;
		}
		static _float f1[4]{}, f2[4]{}, f3[4]{}, f4[4]{}, f5[4]{}, f6[4]{};
		
		memcpy(&f1, &LightMtrl->vLightAmbient,  sizeof(_float4));
		memcpy(&f2, &LightMtrl->vLightDiffuse,  sizeof(_float4));
		memcpy(&f3, &LightMtrl->vLightDir,		sizeof(_float4));
		memcpy(&f4, &LightMtrl->vLightSpecular, sizeof(_float4));
		memcpy(&f5, &LightMtrl->vMtrlAmbient,	sizeof(_float4));
		memcpy(&f6, &LightMtrl->vMtrlSpecular,	sizeof(_float4));

		ImGui::Text(u8"환경광(LightAmbient)");	ImGui::SameLine(), 
		ImGui::SliderFloat4("##1", &f1[0], 0.0f, 1.0f);
		
		ImGui::Text(u8"확산광(vLightDiffuse)");	ImGui::SameLine(), 
		ImGui::SliderFloat4("##2", &f2[0], 0.0f, 1.0f);
		
		ImGui::Text(u8"빛의 방향(vLightDir)");	ImGui::SameLine(), 
		ImGui::SliderFloat4("##3", &f3[0], -1.0f, 1.0f);
		
		ImGui::Text(u8"반사광 하이라이트 (vLightSpecular)"); ImGui::SameLine(), 
		ImGui::SliderFloat4("##4", &f4[0], 0.0f, 1.0f);
		
		ImGui::Text(u8"재질환경광(vMtrlAmbient)"); ImGui::SameLine(),
		ImGui::SliderFloat4("##5", &f5[0], 0.0f, 1.0f);
		
		ImGui::Text(u8"재질반사광(vMtrlSpecular)");ImGui::SameLine(),
		ImGui::SliderFloat4("##6", &f6[0], 0.0f, 1.0f);


		memcpy(&LightMtrl->vLightAmbient, &f1, sizeof(_float4));
		memcpy(&LightMtrl->vLightDiffuse, &f2, sizeof(_float4));
		memcpy(&LightMtrl->vLightDir,	  &f3, sizeof(_float4));
		memcpy(&LightMtrl->vLightSpecular,&f4, sizeof(_float4));
		memcpy(&LightMtrl->vMtrlAmbient,  &f5, sizeof(_float4));
		memcpy(&LightMtrl->vMtrlSpecular, &f6, sizeof(_float4));



		ImGui::TreePop();
	}

}
void CGuiObject::Connect_Model()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	static int32_t iTargetModel = 0;
	if (ImGui::BeginTabBar(u8"탭슛", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"모델"))
		{
			ImGui::RadioButton(u8"첫번째 모델 선택", &m_iModelSelect, 0);  ImGui::SameLine();
			ImGui::RadioButton(u8"두번째 모델 선택", &m_iModelSelect, 1);
			switch (m_iModelSelect)
			{
			case 0:
				if(NULL_TRUE(m_pConnetObjectFirst.lock()))
					m_pConnetObjectFirst = Picking_Object(m_LayerName);
				if( NULL_FALSE(m_pConnetObjectFirst.lock()))
					m_pConnetObjectFirst.lock()->Set_bBoxColor(true);
				break;
			case 1:
				if (NULL_TRUE(m_pConnetObjectSecond.lock()))
					m_pConnetObjectSecond = Picking_Object(m_LayerName);
				if (NULL_FALSE(m_pConnetObjectSecond.lock()))
					m_pConnetObjectSecond.lock()->Set_bBoxColor(true);
				break;
			}

			auto FirstObject = m_pConnetObjectFirst.lock();
			auto SecondObject = m_pConnetObjectSecond.lock();

			if (nullptr == FirstObject)
				ImGui::Text(u8"첫번째 오브젝트가 선택 되지 않음");
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1,0,0,1 });
				ImGui::Text(u8"첫번째 오브젝트 선택 완료"); ImGui::SameLine(); ImGui::Text(FirstObject->Get_PathName().c_str());
				ImGui::PopStyleColor();

			}
				

			if (NULL_TRUE(SecondObject))
				ImGui::Text(u8"두번째 오브젝트가 선택 되지 않음");
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1,0,0,1 });
				ImGui::Text(u8"두번째 오브젝트 선택 완료"); ImGui::SameLine(); ImGui::Text(SecondObject->Get_PathName().c_str());
				ImGui::PopStyleColor();
			}
				
			if (NULL_FALSE(FirstObject) && NULL_FALSE(SecondObject))
			{
				if (ImGui::TreeNode(u8"연결하기"))
				{
					uint32_t iFirstObjectIDNumber = static_pointer_cast<CTriggerObject>(FirstObject)->Get_TriggerInfo().iObjectID;
					uint32_t iFirstObjectTarget   = static_pointer_cast<CTriggerObject>(FirstObject)->Get_TriggerInfo().iTargetObjectID;


					uint32_t iSecondObjectIDNumber = static_pointer_cast<CTriggerObject>(SecondObject)->Get_TriggerInfo().iObjectID;
					uint32_t iSecondObjectTarget   = static_pointer_cast<CTriggerObject>(SecondObject)->Get_TriggerInfo().iTargetObjectID;

					ImGui::RadioButton(u8"첫번째 모델에 두번째 모델을 타겟으로 연결하기", &iTargetModel, 0); 
					ImGui::RadioButton(u8"두번째 모델에 첫번째 모델을 타겟으로 연결하기", &iTargetModel, 1);

					ImGui::Text(u8"첫번째 모델의 ID넘버"); ImGui::SameLine(); ImGui::Text(to_string(iFirstObjectIDNumber).c_str());
					ImGui::Text(u8"첫번째 모델의 타겟 넘버"); ImGui::SameLine(); ImGui::Text(to_string(iFirstObjectTarget).c_str());

					ImGui::Text(u8"두번째 모델의 ID넘버"); ImGui::SameLine(); ImGui::Text(to_string(iSecondObjectIDNumber).c_str());
					ImGui::Text(u8"두번째 모델의 타겟 넘버"); ImGui::SameLine(); ImGui::Text(to_string(iSecondObjectTarget).c_str());


					switch (iTargetModel)
					{
					case 0:
						static_pointer_cast<CTriggerObject>(FirstObject)->Set_TargetIDNumber(iSecondObjectIDNumber);
						static_pointer_cast<CTriggerObject>(SecondObject)->Set_TargetIDNumber(0);

						break;
					case 1:
						static_pointer_cast<CTriggerObject>(SecondObject)->Set_TargetIDNumber(iFirstObjectIDNumber);
						static_pointer_cast<CTriggerObject>(FirstObject)->Set_TargetIDNumber(0);

						break;

					}

					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

	}
}
void CGuiObject::Click_Reset()
{
	if (GetKeyState(VK_RBUTTON) & 0x8000)
	{
		auto mobj = m_pObj.lock();
		if (NULL_FALSE(mobj))
		{
			m_bMouseCheck = false;
			mobj->Set_bBoxColor(false);
			m_pObj.reset();
			m_bCopy = false;
			m_CopyWorld = {};
		}

		auto Fobj = m_pConnetObjectFirst.lock();
		if (NULL_FALSE(Fobj))
		{
			m_bMouseCheck = false;
			Fobj->Set_bBoxColor(false);
			m_pConnetObjectFirst.reset();
		}

		auto Sobj = m_pConnetObjectSecond.lock();
		if (NULL_FALSE(Sobj))
		{
			m_bMouseCheck = false;
			Sobj->Set_bBoxColor(false);
			m_pConnetObjectSecond.reset();
		}
	}
}

void CGuiObject::Add_Decal_Texture()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	
	if (!ImGui::Begin(u8"데칼", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
	{
		ImGui::End(); return;
	}//스크롤 접으면 밑에꺼 하지마라
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	_vector vRight = { 1,0,0,0 }, vUp = { 0, 1, 0 ,0 }, vLook = {}, vPos = {};
	CGameObject::GAMEOBJECT_DESC desc;

	if (ImGui::BeginTabBar(u8"이미지 목록", tab_bar_flags))
	{
		_bool bSetTexture = false;
		int32_t iCount(0),iNameCnt(0),iHorizontalCnt(0);
		ImVec2 scroll_size = ImVec2(0, 0);
		ImGui::BeginChild("Scroll", scroll_size, true, ImGuiWindowFlags_HorizontalScrollbar);
		auto Name = CGameInstance::Get().Get_TextureFileNameList();
		static ImGuiTextFilter filter;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
		//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김
		ImGui::Text(u8"이미지 검색");
		filter.Draw();
			
		for (auto iter : m_GuiResources)
		{
			char ButtonName[256] = "##1";
			if (iNameCnt < Name.size())
			{
				strncat_s(ButtonName, Name[iNameCnt].c_str(), sizeof(char) * 256);

			}
			ImGui::PushID(iCount);
			if (filter.PassFilter(Name[iNameCnt].c_str()))
			{
				if (ImGui::ImageButton(ButtonName, (ImTextureRef)(iter.Get()), ImVec2(128.f, 128.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f),
					ImVec4(0, 0, 0, 0)))
						{
							desc.FileName = Name[iNameCnt];
							desc.bFrontCamera = false;
							desc.eType = MESH_TYPE::DECAL;
							_float4x4 matWorld = {};

							XMStoreFloat4x4(&matWorld, XMLoadFloat4x4(CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW)));
							memcpy(&matWorld.m[0], &vRight, sizeof(_vector));
							memcpy(&matWorld.m[1], &vUp, sizeof(_vector));
							memcpy(&vLook, &matWorld.m[2], sizeof(_vector));
							memcpy(&vPos, &matWorld.m[3], sizeof(_vector));
							XMVector3Normalize(vLook);
							vPos += vLook * 10.f;
							vLook = { 0,0,1,0 };
							memcpy(&desc.matWorld.m[0], &vRight, sizeof(_vector));
							memcpy(&desc.matWorld.m[1], &vUp, sizeof(_vector));
							memcpy(&desc.matWorld.m[2], &vLook, sizeof(_vector));
							memcpy(&desc.matWorld.m[3], &vPos, sizeof(_vector));

							CGameInstance::Get().Add_Direct_Decal_Texture(Name[iNameCnt], iter);
							CGameInstance::Get().Add_GameObject_toLayer(ETOUI(m_eLevel), L"OBJ_Decal", ETOUI(m_eLevel), L"Layer_Decal", &desc);

						}

				if (iHorizontalCnt < 5)
					ImGui::SameLine();
				else iHorizontalCnt = 0;

				++iHorizontalCnt;
				++iCount;
				++iNameCnt;


			}
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CGuiObject::Multy_Copy()
{
	auto pObj = m_pObj.lock();
	if (NULL_TRUE(pObj))
	{
		ImGui::Text(u8"오브젝트가 선택되지 않음");
		return;
	}

	GAMEOBJECT_DESC desc{};
	int32_t iMultiyCopy(0);
	static int32_t iCopyX =1, iCopyY = 1, iCopyZ = 1;
	static _bool bCopyX= false, bCopyY = false, bCopyZ = false, bCopyFloat = false;
	static _float3 fwh{};
	
	ImGui::PushItemWidth(200);

	if (ImGui::InputInt(u8"복사할 X칸수##Copy1", &iCopyX))
		bCopyX = true;
	else bCopyX = false;

	if (ImGui::InputInt(u8"복사할 Z칸수##Copy2", &iCopyZ))
		bCopyZ = true;
	else bCopyZ = false;
	
	if (ImGui::InputInt(u8"복사할 Y칸수##Copy3", &iCopyY))
		bCopyY = true;
	else bCopyY = false;

	ImGui::InputFloat3(u8"X, Y, Z 간격", (_float*)&fwh);


	ImGui::PopItemWidth();

	if (fwh.x == 0 && fwh.y == 0)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1,0,0,1 });
		ImGui::Text(u8"넓이가 설정되지 않음");
		ImGui::PopStyleColor();
	}

	ImGui::RadioButton(u8"현재 상태로 붙여 넣기", &iMultiyCopy, 1);

	if (iCopyX < 0)
		iCopyX = 0;
	if (iCopyY < 0)
		iCopyY = 0;
	if (iCopyZ < 0)
		iCopyZ = 0;
	if(ImGui::Button(u8"값 초기화")) 
	{
		iCopyX = 0;
		iCopyY = 0;
		iCopyZ = 0;
		iMultiyCopy = (0.f, 0.f);
	}
	ImGui::SameLine(200.f);

	if (ImGui::Button(u8"레이어 초기화"))
	{
		CGameInstance::Get().Reset_CopyLayer();
	}

	if (iCopyX || iCopyY || iCopyZ)
	{
		CGameInstance::Get().Reset_CopyLayer();

		switch (pObj->Get_MeshType())
		{
		case MESH_TYPE::NONANIME:
			break;
		case MESH_TYPE::TRIGGER:
			m_CopyTriggerName = static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerInfo().strTriggerName;
			break;
		}
		
		XMStoreFloat4x4(&m_CopyWorld, pObj->Get_Transform().lock()->Get_World());

		desc.FileName = pObj->Get_PathName();			//매쉬 경로 복사		
		desc.bCopy = true;						//복사 한다는 뜻
		desc.eType = pObj->Get_MeshType();			// 매쉬 타입
		desc.strTriggerName = m_CopyTriggerName; //트리거 밸류 저장

		_float4 fPos{}, fPosOrigin{};
		memcpy(&fPosOrigin, &m_CopyWorld.m[3],sizeof(_float4));
		int32_t x(0), z(0), y(0);
		x = max(1, iCopyX);
		y = max(1, iCopyY);
		z = max(1, iCopyZ);

		for (uint32_t i = 0; i < x; ++i)
		{
			for (uint32_t j = 0; j < y; ++j)
			{
				for (uint32_t k = 0; k < z; ++k)
				{
					_float4 fCopyPos{};
					fCopyPos = fPosOrigin;

					
					fCopyPos.x = fPosOrigin.x + i * fwh.x;
					fCopyPos.y = fPosOrigin.y + j * fwh.y;
					fCopyPos.z = fPosOrigin.z + k * fwh.z;
					if (x == 0 && y == 0 && z == 0 || (fCopyPos.x == fPosOrigin.x && fCopyPos.y == fPosOrigin.y && fCopyPos.z == fPosOrigin.z))
						continue;

					memcpy(&m_CopyWorld.m[3], &fCopyPos, sizeof(_float4));

					desc.matWorld = m_CopyWorld;
					switch (pObj->Get_MeshType())
					{
					case MESH_TYPE::NONANIME:
						CGameInstance::Get().Add_GameObject_ToCopyLayer(ETOUI(m_eLevel), L"OBJ_WorldObject", &desc);
						break;
					case MESH_TYPE::TRIGGER:
						desc.bTrigger = static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerInfo().bOtherTrigger;
						CGameInstance::Get().Add_GameObject_ToCopyLayer(ETOUI(m_eLevel), L"OBJ_Trigger", &desc);
						break;
					}
				}
			}
		}
			
		
		m_bCopy = false;
		m_CopyTriggerName = "";
		m_CopyWorld = {};
	
	}

	if (iMultiyCopy)
	{	switch (pObj->Get_MeshType())
		{
		case MESH_TYPE::NONANIME:
			CGameInstance::Get().Move_CopyLayer_ToObjectLayer(ETOUI(m_eLevel), L"Layer_WorldObject");
			break;
		case MESH_TYPE::TRIGGER:
			CGameInstance::Get().Move_CopyLayer_ToObjectLayer(ETOUI(m_eLevel), L"Layer_TriggerObject");
			break;
		}

		m_bMouseCheck = false;
		pObj->Set_bBoxColor(false);
		pObj.reset();

		iCopyX = 0;
		iCopyY = 0;
		iCopyZ = 0;

		iMultiyCopy = (0.f, 0.f);
	}
}

void CGuiObject::Move_Layer()
{
	 char LayerName[256] = {};
	const _tchar* LayerMove = {};
	 _bool		bMove = false;
	 WideCharToMultiByte(CP_ACP, 0, m_LayerName.data(),-1, LayerName, sizeof(char)*256, NULL, NULL);

	ImGui::Text(u8"현재 레이어 : "); ImGui::SameLine(); ImGui::Text(LayerName);

	auto pObj = m_pObj.lock();
	if (NULL_TRUE(pObj))
	{
		ImGui::Text(u8"오브젝트가 선택되지 않음");
		return;
	}
		
	ImGui::Text(u8"이동할 레이어 선택");
	if (ImGui::Button(u8"오브젝트 레이어"))
		LayerMove = L"Layer_WorldObject";

	if (ImGui::Button(u8"트리거 레이어"))
		LayerMove = L"Layer_TriggerObject";

	if (ImGui::Button(u8"데칼 레이어"))
		LayerMove = L"Layer_Decal";

	if (ImGui::Button(u8"임시 레이어"))
		LayerMove = L"Layer_Temp";

	if (NULL_FALSE(LayerMove))
	{
		pObj->Set_bBoxColor(false);
		CGameInstance::Get().Move_To_Layer(ETOUI(m_eLevel), m_LayerName.c_str(), LayerMove, pObj);
		m_bMouseCheck = false;
		m_pObj.reset();
		m_bCopy = false;
		m_CopyWorld = {};
	}

}

void CGuiObject::Navi_Creator()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김


	if (!ImGui::Begin(u8"네비", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
	{
		ImGui::End(); return;
	}//스크롤 접으면 밑에꺼 하지마라

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar(u8"목록", tab_bar_flags))
	{
		POINT tMouse{};
		GetCursorPos(&tMouse);
		ScreenToClient(g_hWnd, &tMouse);
		static _float3 fPos[3]{};
		static _bool   A{ false }, B{ false }, C{ false }, D{ false }, Check{ false }, bSelect{ false }, bOnlyTwo{ false };
		ImGui::Text(u8"이벤트 선택");
		const char* items[] = { "FIRST","SECOND","BOSS_TP","END" };
		const _char* pTwogae = nullptr;
		static CELL_EVENT event_Select_Index = CELL_EVENT::END;
		
		static int32_t iSelect{0};
		
		const char* combo_preview_value = items[ETOUI(event_Select_Index)];
		if (ImGui::BeginCombo(u8"옵션", combo_preview_value, ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_WidthFitPreview))
		{
			for (int i = 0; i < IM_COUNTOF(items); ++i)
			{
				const bool is_selected = (ETOUI(event_Select_Index) == i);
				if (ImGui::Selectable(items[i], is_selected)) // 선택한 문자열
					event_Select_Index = static_cast<CELL_EVENT>(i);

				if (is_selected)
					ImGui::SetItemDefaultFocus();
				//선택된 아이템을 포커스하라
			}
			ImGui::EndCombo();
		}
		if (CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::X))
			bOnlyTwo = !bOnlyTwo;

		ImGui::RadioButton(u8"설치", &iSelect, 0);
		ImGui::RadioButton(u8"선택", &iSelect, 1);
		pTwogae = (bOnlyTwo == true)  ? pTwogae = "Enable" : pTwogae = "Disable";

		ImGui::Text(u8"두개만 : %s", pTwogae);
		_matrix CameProj = XMLoadFloat4x4(CGameInstance::Get().Get_Transform(D3DTS::PROJ));
		_float4x4 Proj = {};
		XMStoreFloat4x4(&Proj, CameProj);

		_float2 ViewPort = CGameInstance::Get().Get_ViewportSize();
		_float rayX = (2.f  * tMouse.x / ViewPort.x - 1.f) / Proj(0, 0);
		_float rayY = (-2.f * tMouse.y / ViewPort.y+ 1.f) / Proj(1, 1);

		//뷰포트에서의 광선 정의9
		_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		_vector rayDir = XMVectorSet(rayX, rayY, 1.f, 0.f);

		//월드 좌표로 변환
		_matrix CamView = XMLoadFloat4x4(CGameInstance::Get().Get_Transform(D3DTS::VIEW));
		_matrix InverseView = XMMatrixInverse(nullptr, CamView);

		rayOrigin = XMVector3TransformCoord(rayOrigin, InverseView);
		rayDir = XMVector3Normalize(XMVector3TransformNormal(rayDir, InverseView));

		_float fMax = { FLT_MAX };
		_float tDis = 0;
		_float t1Dis = 0;
		_float3 LastPos{};
		_vector TriFirst[3]{XMVectorSet(0,0,0,0),
			                XMVectorSet(0,0,(TERRIANZ * TERRIANZ) *6 ,0),
			                XMVectorSet((TERRIANZ * TERRIANZ)*6 ,0,(TERRIANZ * TERRIANZ) *6,0) };

		_vector TriSecond[3]{ XMVectorSet((TERRIANZ * TERRIANZ)*6,0,(TERRIANZ * TERRIANZ)*6 ,0),
							  XMVectorSet((TERRIANZ * TERRIANZ) * 6,0,0,0),
							  XMVectorSet(0,0,0,0) };
		
			if (iSelect == 0)
			{
				if (TriangleTests::Intersects(rayOrigin, rayDir, TriFirst[0], TriFirst[1], TriFirst[2], tDis))
				{
					XMStoreFloat3(&LastPos, rayOrigin + rayDir * tDis);
					fMax = tDis;
				}

				if (TriangleTests::Intersects(rayOrigin, rayDir, TriSecond[0], TriSecond[1], TriSecond[2], t1Dis))
				{
					if (fMax > t1Dis)
					{
						XMStoreFloat3(&LastPos, rayOrigin + rayDir * t1Dis);
					}
				}
			}

			LastPos.y = 0;
			if (iSelect == 0)
			{
				if (CGameInstance::Get().Get_DIMouseOneClick(DIMK::RBUTTON, ENGINE_MOUSE::A_CLICK))
					A = B = C = D=false;

				bSelect = true;
				auto pCell = m_pCell.lock();
				if (NULL_FALSE(pCell))
				{
					pCell->Set_Choice(false);
					m_pCell.reset();
				}

				ImGui::Text("X %f.3f:", LastPos.x);
				ImGui::Text("Z %f.3f :", LastPos.z);

					if (CGameInstance::Get().Get_DIMouseOneClick(DIMK::LBUTTON, ENGINE_MOUSE::A_CLICK))
					{
						if (!A)
						{
							
							CGameInstance::Get().Check_NeraPos(&LastPos);
							fPos[0] = LastPos;
							A = true;
						}
						else if (!B)
						{
							CGameInstance::Get().Check_NeraPos(&LastPos);
							fPos[1] = LastPos;
							B = true;
						}
						else if (!C)
						{
							if (!bOnlyTwo)
							{
								CGameInstance::Get().Check_NeraPos(&LastPos);
							}
							C = true;

							fPos[2] = LastPos;
						}

						
						if(C)
						{
							CGameInstance::Get().Add_NaviMeshInfo(&fPos[0], event_Select_Index);
							A = B = C = false;
						}

					}
				
		

				if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) && CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::Z))
					CGameInstance::Get().Undo_Cell();
				A ? ImGui::Text(u8"첫번째점 선택 완료 x : %3.f, z : %3.f", fPos[0].x, fPos[0].z) : ImGui::Text(u8"첫번째점 선택되지 않음");
				B ? ImGui::Text(u8"두번째점 선택 완료 x : %3.f, z : %3.f", fPos[1].x, fPos[1].z) : ImGui::Text(u8"두번째점 선택되지 않음");
				C ? ImGui::Text(u8"세번째점 선택 완료 x : %3.f, z : %3.f", fPos[2].x, fPos[2].z) : ImGui::Text(u8"세번째점 선택되지 않음");
				
			}
			else if (iSelect == 1)
			{
				auto pObj = m_pCell.lock();


				if (NULL_TRUE(pObj) && CGameInstance::Get().Get_DIMouseOneClick(DIMK::LBUTTON, ENGINE_MOUSE::HOLD))
					m_pCell = CGameInstance::Get().Select_TriAngle(rayOrigin, rayDir);
	
				if (NULL_TRUE(pObj))
					ImGui::Text(u8"선택 안됨");
				else
				{
					ImGui::Text(u8"선택 됨");
					pObj->Set_Choice(true);
				}

				if (CGameInstance::Get().Get_DIMouseOneClick(DIMK::RBUTTON, ENGINE_MOUSE::A_CLICK))
				{
					if (NULL_FALSE(pObj))
					{
						pObj->Set_Choice(false);
						m_pCell.reset();
					}
					A = B = C = D =false;
				}

				if (NULL_FALSE(pObj))
				{

					ImGui::Text(u8"현재 인덱스 번호 : %d", pObj->Get_NaviInfo().iIndex); ImGui::SameLine(150.f);
					const _char* pName = nullptr;
					if (pObj->Get_Event() == CELL_EVENT::FIRST)
						pName = "FIRST";
					if (pObj->Get_Event() == CELL_EVENT::SECOND)
						pName = "SECOND";
					if (pObj->Get_Event() == CELL_EVENT::BOSSTP)
						pName = "BOSS_TP";
					if (pObj->Get_Event() == CELL_EVENT::END)
						pName = "END";

					ImGui::Text(u8"현재 이벤트 상태 : %s ", pName);

					if (NULL_FALSE(pObj) && CGameInstance::Get().Get_DIKeyState(DIK_DELETE))
					{
						pObj->Get_NaviInfo().bDead = true;
						m_pCell.reset();
					}
					if (NULL_FALSE(pObj))
					{
						pObj->Set_CellEvent(event_Select_Index);
					//	m_pCell.reset();
					}
						
				}
			}
			if(CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::F7))
				CGameInstance::Get().Ready_Neightbors();

			if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) && CGameInstance::Get().Get_DIKeyState(DIK_F1))
				CGameInstance::Get().Save_Navi(L"../../Navi.json", "Navi");
			if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) && CGameInstance::Get().Get_DIKeyState(DIK_F7))
				CGameInstance::Get().Load_Navi(L"../../Navi.json", "Navi");

		
		ImGui::EndTabBar();
	}


	ImGui::End();
}

void CGuiObject::Load_Data()
{
	if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('L') & 0x8000)
	{
		Load_Value(m_eLevel, m_strGameObjectPath,  L"Layer_WorldObject", L"OBJ_WorldObject", m_strGameObject);
		Load_Value(m_eLevel, m_strTriggerPath,     L"Layer_TriggerObject", L"OBJ_Trigger", m_strTrigger);
		Load_Value(m_eLevel, m_strDecalpath,        L"Layer_Decal", L"OBJ_Decal", m_strDecal);
	}
		
}

HRESULT CGuiObject::Load_Value(LEVEL eLevel, const _wstring strFilePath, const _wstring strLayerName, const _wstring strObjName, const string strLoadDataName)
{
		json j;
		ifstream file(strFilePath);
		if (!file.is_open())
		{
			MSG_BOX("로드할 파일이 없음");
			return E_FAIL;
		}
		j = json::parse(file);
		int32_t index = 0;
		for (auto& iter : j[strLoadDataName])
		{
			GAMEOBJECT_DESC desc{};
			desc.FileName = { "" };
			desc.j = iter;
			desc.index = index++;
			desc.eType = iter["MeshType"];
			if (iter["TriggerValue"] != nullptr)
			{
				desc.strTriggerName = iter["TriggerValue"];
 				desc.bTrigger = iter["OtherTriggerValue"];
			}
				
			CGameInstance::Get().Add_GameObject_toLayer(ETOUI(eLevel), strObjName,
				ETOUI(eLevel), strLayerName, &desc);

		}

	file.close();

	MSG_BOX("로드 된듯?");
	return S_OK;
}

void CGuiObject::Save()
{
	if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('Q') & 0x8000)
	{
		CGameInstance::Get().Save_Data(ETOUI(m_eLevel), m_strGameObjectPath, L"Layer_WorldObject", m_strGameObject);
		CGameInstance::Get().Save_Data(ETOUI(m_eLevel), m_strTriggerPath, L"Layer_TriggerObject", m_strTrigger);
		CGameInstance::Get().Save_Data(ETOUI(m_eLevel), m_strDecalpath,    L"Layer_Decal", m_strDecal);
	}
		
}
unique_ptr<CGuiObject> CGuiObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CGuiObject>(new CGuiObject(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CGuiObject::Clone(void* pArg)
{
	auto pInstance = shared_ptr <CGameObject>(new CGuiObject(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
