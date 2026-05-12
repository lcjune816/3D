#include "GameInstance.h"
#include "Boss_Teacher.h"
#include "Loader_Defines.h"
CBoss_Teacher::CBoss_Teacher(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CBoss_Teacher::CBoss_Teacher(const CBoss_Teacher& Prototye) : CGameObject(Prototye)
{
}
CBoss_Teacher::~CBoss_Teacher()
{
};

HRESULT CBoss_Teacher::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Boss/Teacher/Teacher.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;


	m_pStateMachine = static_pointer_cast<CFSM_Machine>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Machine", nullptr));
	if (NULL_TRUE(m_pStateMachine)) return E_FAIL;


	_matrix mat = XMMatrixIdentity();
	mat = XMMatrixScaling(0.9f,0.9f,0.9f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);

	CTeacher_FSM::FSM_TEACHER_DESC pDesc;
	pDesc.pTeacher = SHARED_THIS(CBoss_Teacher);
	pDesc.pOtherMatrix = m_pOtherMatrix;
	auto idle = static_pointer_cast<CFSM_Teacher_IDLE>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Idle"), &pDesc));
	if(NULL_TRUE(idle))
		return E_FAIL;
	
	auto Move = static_pointer_cast<CFSM_Teacher_IDLE>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::GAMEPLAY), TEXT("FSM_Teacher_Move"),&pDesc));
	if (NULL_TRUE(Move))
		return E_FAIL;
	
	m_pStateMachine->Add_State(FSM::IDLE, idle);
	m_pStateMachine->Add_State(FSM::MOVE, Move);

	m_pStateMachine->Set_Owner(SHARED_THIS(CBoss_Teacher));
	m_pStateMachine->Change_State(FSM::IDLE);

	return S_OK;

}
void CBoss_Teacher::State_Move()
{
	if (CGameInstance::Get().RayCast(ETOUI(LEVEL::END), L"Layer_WorldObject",L"Layer_Player", "Player", m_pTransform, XMVectorSet(0.f,5.f,0.f,1.f)))
	{
		m_eBoss.bMove = false;
	}
	else 
	{
		m_eBoss.bMove = true;
		m_pStateMachine->Change_State(FSM::MOVE);
	}

}
HRESULT CBoss_Teacher::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CBoss_Teacher::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 5.f;
	desc.m_fSpeedPerSec = 30.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	CGameObject* pObj = CGameInstance::Get().Get_ObjectPtr(ETOUI(LEVEL::GAMEPLAY), TEXT("Layer_Player"), "Player");
	m_pOtherMatrix = pObj->Get_Transform().lock()->Get_WorldPtr();

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	if (NULL_TRUE(pObj))
		return E_FAIL;

	CGameInstance::Get().Add_LightMtrl(m_PathName);
	m_pTransform->Set_State(STATE::POS, XMVectorSet(0.f, 0.f, 5.f, 1.f));
	//플레이어 Look하고
	//플레이어가 보스한테 raycast해서
	//그거 두개 내적하면 앞뒤 판정은 되는데
	//중간에 벽끼면? 그거먼저 체크해서 있으면 skip 해야되네
	return S_OK;
}
void CBoss_Teacher::Priority_Update(_float fTimeDelta)
{

}
void CBoss_Teacher::Update(_float fTimeDelta)
{

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	string name = Model_Animation(m_pAnimator->Get_NameList());
	if (name != "")
		m_pAnimator->Change_Animation(name);

	State_Move();
	m_pAnimator->Update(fTimeDelta);
	m_pStateMachine->Update_Machine(fTimeDelta);
	m_pAnimator->CalculateFinalBoneMatrices();
	m_bFinished = m_pAnimator->Animation_End();

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CBoss_Teacher));


}
void CBoss_Teacher::Late_Update(_float fTimeDelta)
{
}
HRESULT CBoss_Teacher::Render()
{
	for (uint32_t i = 0; i < BONE_MATRIX; ++i)
		XMStoreFloat4x4(&m_bones[i], XMMatrixIdentity());

	vector<_float4x4> bBone = m_pAnimator->Get_FinalBoneMatrix();
	for (uint32_t i = 0; i < m_pAnimator->Get_BoneCnt(); ++i)
	{
		m_bones[i] = bBone[i];

	}

	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pShaderCom->Bind_Matrix_Array("g_Bone", &m_bones[0], BONE_MATRIX);
	for (auto iter : m_pMeshList)
	{
		iter->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
		m_pShaderCom->Begin(0);
		iter->Bind_Resource();
		iter->Render();

	}
	return S_OK;
}
string CBoss_Teacher::Model_Animation(const vector<string>& pNames)
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


void CBoss_Teacher::Change_Animation(TEACHER_ANIME eAnime, _bool bLoop)
{
	if (m_bOnlyActionState)
		return;

	m_bFinished = bLoop;
	m_pAnimator->Change_Animation_Enum(ETOUI(eAnime), bLoop);
	m_eAnimeState = eAnime;
}
unique_ptr<CBoss_Teacher> CBoss_Teacher::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CBoss_Teacher>(new CBoss_Teacher(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CBoss_Teacher::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CBoss_Teacher>(new CBoss_Teacher(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
