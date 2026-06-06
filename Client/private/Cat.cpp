#include "GameInstance.h"
#include "Cat.h"
#include "Loader_Defines.h"
CBoss_Cat::CBoss_Cat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CBoss_Cat::CBoss_Cat(const CBoss_Cat& Prototye) : CGameObject(Prototye)
{
}
CBoss_Cat::~CBoss_Cat()
{
};

HRESULT CBoss_Cat::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Boss/castsaagi/catNormal.fbx";
	//importModel.pFile = "../../Resource/Boss/Teacher/SK_CustomBody.fbx";
	//importModel.pFile = "../../Resource/Boss/Teacher/Avatar_Kiana_C8_WS.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;

	CNavigation::NAVIGATION_DESC NaviDesc;
	NaviDesc.iIndex = 0;
	NaviDesc.eOwner = OWNER::BOSS;
	//0
	//230
	if (FAILED(Add_Component(ETOUI(LEVEL::GASZONE), TEXT("Component_Navigation"), TEXT("Com_Navigation"), m_pNavigation, &NaviDesc)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("FSM_Machine"), TEXT("FSM_Machine"), m_pStateMachine, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"), TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	
	CCat_Fog::CATFOG_DESC FogDesc{};
	FogDesc.pParentMatrix = m_pTransform->Get_WorldPtr();
	m_pFogEffect = static_pointer_cast<CCat_Fog>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GASZONE), TEXT("OBJ_CatFog"),&FogDesc));

	_matrix mat = XMMatrixIdentity();
	mat = XMMatrixScaling(0.9f, 0.9f, 0.9f) * XMMatrixRotationY(XMConvertToRadians(180.f));
 	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList[ETOUI(CATFORM::NORMAL)], m_pAnimator[ETOUI(CATFORM::NORMAL)], m_pTransform, mat);

	importModel.pFile = "../../Resource/Boss/castsaagi/nightmare.fbx";

	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList[ETOUI(CATFORM::NIGHTMARE)], m_pAnimator[ETOUI(CATFORM::NIGHTMARE)], m_pTransform, mat);

	m_pStateMachine->Set_Owner(SHARED_THIS(CBoss_Cat));
	m_pStateMachine->Add_State(FSM::SPAWN, static_pointer_cast<CFSM_Teacher_Spawn>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GASZONE), TEXT("FSM_Cat_Spawn"), nullptr)));
	m_pStateMachine->Add_State(FSM::MOVE, static_pointer_cast<CFSM_Teacher_Spawn>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GASZONE), TEXT("FSM_Cat_Nightmare"), nullptr)));
	m_pStateMachine->Change_State(FSM::SPAWN);
	m_pAnimator[ETOUI(m_eFormType)]->Stop_Animation(true);
	_vector vPos = m_pNavigation->Find_CellPos(NaviDesc.iIndex);
	m_pTransform->Set_State(STATE::POS, XMVectorSetW(vPos, 1.f));
	return S_OK;

}

HRESULT CBoss_Cat::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CBoss_Cat::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 60.f;
	desc.m_fSpeedPerSec = 30.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	
	m_Components.emplace(L"Com_Animator_Normal", m_pAnimator[ETOUI(CATFORM::NORMAL)]);
	m_Components.emplace(L"Com_Animator_Nightmare", m_pAnimator[ETOUI(CATFORM::NIGHTMARE)]);
	m_pTransform->Rotation(XMVectorSet(0, 1, 0, 0), -90.f);

	strcpy_s(m_pTagName, "Boss_Cat");
	return S_OK;
}
void CBoss_Cat::Priority_Update(_float fTimeDelta)
{
	if (CGameInstance::Get().Get_DIKeyState(DIK_K))
	{
		m_eFormType = CATFORM::NIGHTMARE;
	}
	if (CGameInstance::Get().Get_DIKeyState(DIK_L))
	{
		m_eFormType = CATFORM::NORMAL;
	}
}
void CBoss_Cat::Update(_float fTimeDelta)
{
	if (CGameInstance::Get().Get_DIKeyState(DIK_M))
	{
		EVENT eEvent{};
		eEvent.eEvent = WORLD_EVENT::BOSS_SPAWN;
		CGameInstance::Get().Notify(WORLD_EVENT::BOSS_SPAWN, eEvent);
	}
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	string name = Model_Animation(m_pAnimator[ETOUI(m_eFormType)]->Get_NameList());
	if (name != "")
		m_pAnimator[ETOUI(m_eFormType)]->Change_Animation(name);

	m_pAnimator[ETOUI(m_eFormType)]->Update(fTimeDelta);
	m_pStateMachine->Update_Machine(fTimeDelta);
	m_bFinished = m_pAnimator[ETOUI(m_eFormType)]->Animation_End();
	m_pFogEffect->Update(fTimeDelta);
	m_pTransform->Set_State(STATE::POS, m_pNavigation->SetUp_OnNavigation(m_pTransform->Get_State(STATE::POS), 1.f));
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CBoss_Cat));


}
void CBoss_Cat::Late_Update(_float fTimeDelta)
{
}
HRESULT CBoss_Cat::Render()
{
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

	m_pAnimator[ETOUI(m_eFormType)]->Bind_Resource_BoneMatrix(m_pShaderCom.get(), "g_Bone");
	for (auto iter : m_pMeshList[ETOUI(m_eFormType)])
	{
		iter->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
		m_pShaderCom->Begin(0);
		iter->Bind_Resource();
		iter->Render();

	}
	m_pFogEffect->Render();
	//m_pNavigation->Render();
	return S_OK;
}
string CBoss_Cat::Model_Animation(const vector<string>& pNames)
{
	if (!ImGui::Begin(u8"보스 애니메이션", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
	{
		ImGui::End(); return "";
	}//스크롤 접으면 밑에꺼 하지마라

	static string name;
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar(u8"탭슛", tab_bar_flags))
	{
		static _bool h_borders = true;
		static _bool v_borders = true;
		static int32_t columns_count = 4;
		static ImGuiTextFilter filter;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
		//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김


		if (ImGui::TreeNode(u8"선택"))
		{
			if (pNames.empty())
			{
				ImGui::Text(u8"애니메이션이 없음");
				ImGui::TreePop();

				ImGui::EndTabBar();

				ImGui::End();
				return "";
			}
			auto& iter = pNames;
			const  int32_t lines_count = iter.size();
			ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
			if (columns_count < 2)
				columns_count = 2;
			ImGui::SameLine(); ImGui::Checkbox(u8"가로줄", &h_borders);
			ImGui::SameLine(); ImGui::Checkbox(u8"세로줄", &v_borders);
			ImGui::SameLine(); ImGui::Text(name.c_str());

			ImGui::Columns(columns_count, NULL, v_borders);
			filter.Draw();
			for (int i = 0; i < lines_count; ++i)
			{
				if (h_borders && ImGui::GetColumnIndex() == 0)
					ImGui::Separator();

				ImGui::PushID(i);
				if (filter.PassFilter(iter[i].c_str()))
				{
					ImGui::Text(iter[i].c_str(), ImGui::GetColumnWidth());
					if (ImGui::Button(u8"선택"))
					{
						name = iter[i];
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
		ImGui::EndTabBar();
	}

	ImGui::End();

	return name;
}


void CBoss_Cat::Change_Animation(CAT_ANIME eAnime, _bool bLoop, _bool bForce)
{
	if (m_bOnlyActionState)
		return;

	m_bFinished = bLoop;
	m_pAnimator[ETOUI(m_eFormType)]->Change_Animation_Enum(ETOUI(eAnime), bLoop, bForce);
}
void CBoss_Cat::Change_Animation_Nightmare(CAT_ANIME_NIGHTMARE eAnime, _bool bLoop, _bool bForce)
{

	if (m_bOnlyActionState)
		return;

	m_bFinished = bLoop;
	m_pAnimator[ETOUI(m_eFormType)]->Change_Animation_Enum(ETOUI(eAnime), bLoop, bForce);
}
unique_ptr<CBoss_Cat> CBoss_Cat::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CBoss_Cat>(new CBoss_Cat(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CBoss_Cat::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CBoss_Cat>(new CBoss_Cat(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
