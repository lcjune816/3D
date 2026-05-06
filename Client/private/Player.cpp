#include "GameInstance.h"
#include "Player.h"
#include "Player_FSM.h"
#include "FSM_Idle.h"
#include "FSM_Move.h"
#include "FSM_Jump.h"
#include "FSM_Crouch.h"
#include "FSM_Hand.h"
#include "FSM_LeftHand.h"
#include "FSM_RightHand.h"
#include "Player_LeftHand.h"
#include "Player_RightHand.h"
CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CPlayer::CPlayer(const CPlayer& Prototye) : CGameObject(Prototye)
{
}
CPlayer::~CPlayer()
{
};

HRESULT CPlayer::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Character/Player/PlayerGrab5.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;

	for (uint32_t i = 0; i < ETOUI(PLAYER_MACHINE::END); ++i)
	{
		m_pStateMachine[i] = static_pointer_cast<CFSM_Machine>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Machine", nullptr));
		if (NULL_TRUE(m_pStateMachine)) return E_FAIL;

	}
	
	auto Idle = static_pointer_cast<CFSM_Idle>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Idle", nullptr));
	if(NULL_TRUE(Idle)) return E_FAIL;

	auto Move = static_pointer_cast<CFSM_Move>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Move", nullptr));
	if (NULL_TRUE(Move)) return E_FAIL;

	auto Jump = static_pointer_cast<CFSM_Jump>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Jump", nullptr));
	if (NULL_TRUE(Jump)) return E_FAIL;
	
	auto Crouch = static_pointer_cast<CFSM_Crouch>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Crouch", nullptr));
	if (NULL_TRUE(Crouch)) return E_FAIL;
	
	auto FsmLeftHand = static_pointer_cast<CFSM_LeftHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_LeftHand", nullptr));
	if (NULL_TRUE(FsmLeftHand)) return E_FAIL;

	auto FsmRightHand = static_pointer_cast<CFSM_RightHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_RightHand", nullptr));
	if (NULL_TRUE(FsmRightHand)) return E_FAIL;

	auto RHand = static_pointer_cast<CPLayer_RightHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_PlayerRightHand", nullptr));
	if (NULL_TRUE(RHand)) return E_FAIL;

	auto LHand = static_pointer_cast<CPlayer_LeftHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_PlayerLeftHand", nullptr));
	if (NULL_TRUE(LHand)) return E_FAIL;

	LHand->Connet_Player(SHARED_THIS(CPlayer));
	RHand->Connet_Player(SHARED_THIS(CPlayer));

	_matrix mat = XMMatrixIdentity();
	
	mat =  XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);

	m_pPlayerLHand = LHand;
	m_pPlayerRHand = RHand;

	FsmRightHand->Set_RightHand(m_pPlayerRHand);
	FsmLeftHand->Set_LeftHand(m_pPlayerLHand);
	
	for (uint32_t i = 0; i < ETOUI(PLAYER_MACHINE::END); ++i)
		m_pStateMachine[i]->Set_Owner(SHARED_THIS(CPlayer));

	m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Add_State(FSM::IDLE, Idle);
	m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Add_State(FSM::MOVE, Move);
	m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Add_State(FSM::JUMP, Jump);
	m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Add_State(FSM::CROUCH, Crouch);
	m_pStateMachine[ETOUI(PLAYER_MACHINE::LEFT_HAND)]->Add_State(FSM::HAND, FsmLeftHand);
	m_pStateMachine[ETOUI(PLAYER_MACHINE::RIGHT_HAND)]->Add_State(FSM::HAND, FsmRightHand);

	m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Change_State(FSM::IDLE);

	return S_OK;

}
void CPlayer::State_Move()
{
	m_ePlayer.bMove = false;
	m_ePlayer.bCrouch = false;


	if (CGameInstance::Get().Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		m_ePlayer.bCrouch = true;
	}
	if (CGameInstance::Get().Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_eState = MOVE::LEFT;
		m_ePlayer.bMove = true;
	}
	else if (CGameInstance::Get().Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_eState = MOVE::RIGHT;
		m_ePlayer.bMove = true;
	}
	else if (CGameInstance::Get().Get_DIKeyState(DIK_UP) & 0x80)
	{
		m_eState = MOVE::FORWARD;
		m_ePlayer.bMove = true;
	}
	else if (CGameInstance::Get().Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		m_eState = MOVE::BACKWARD;
		m_ePlayer.bMove = true;
	}
	else
	{
		m_eState = MOVE::IDLE;
	}
		
	if (CGameInstance::Get().Get_DIKeyState(DIK_SPACE) & 0x80 && !m_ePlayer.bJump)
	{
		m_eState = MOVE::JUMP;
		m_ePlayer.bJump = true;
	}
	
	///////////////////손//////////////////////

	if (!m_ePlayer.bLHand && CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80)
	{
		m_ePlayer.bLHand = true;
		m_ePlayer.bHand = true;

		m_pStateMachine[ETOUI(PLAYER_MACHINE::LEFT_HAND)]->Change_State(FSM::HAND);
	}
	if (!m_ePlayer.bRHand && CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80)
	{
		m_ePlayer.bRHand = true;
		m_ePlayer.bHand = true;
		m_pStateMachine[ETOUI(PLAYER_MACHINE::RIGHT_HAND)]->Change_State(FSM::HAND);
	}
		

	//////////////기본 동작/////////////////////
	if (m_ePlayer.bFalling)
		return;


	if (m_ePlayer.bJump )
	{
		m_ePlayer.bFalling = true;
		m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Change_State(FSM::JUMP);
		return;
	}

	if (m_ePlayer.bCrouch)
	{
		m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Change_State(FSM::CROUCH);
		return;
	}

	if (m_ePlayer.bMove)
	{
		if (CGameInstance::Get().Get_DIKeyState(DIK_LSHIFT) & 0x80)
		{
			m_ePlayer.bRun = true;
			m_pTransform->Velocity_Speed(10.f);
		}
		else
			m_ePlayer.bRun = false;
		m_pStateMachine[ETOUI(PLAYER_MACHINE::NORMAL)]->Change_State(FSM::MOVE);
	}

}
HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CPlayer::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 30.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if(FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	CGameInstance::Get().Add_LightMtrl(m_PathName);
	return S_OK;
}
void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pPlayerRHand->Priority_Update(fTimeDelta);
}
void CPlayer::Update(_float fTimeDelta)
{

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	string name = Model_Animation(m_pAnimator->Get_NameList());
	if (name != "")
		m_pAnimator->Change_Animation(name);

	State_Move();
 	m_pAnimator->Update(fTimeDelta);

	for (uint32_t i = 0; i < ETOUI(PLAYER_MACHINE::END); ++i)
		m_pStateMachine[i]->Update_Machine(fTimeDelta);

	m_pAnimator->CalculateFinalBoneMatrices();
	m_bFinished = m_pAnimator->Animation_End();

	m_pPlayerRHand->Update(fTimeDelta);
	m_pPlayerLHand->Update(fTimeDelta);
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, SHARED_THIS(CPlayer));


}
void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pPlayerRHand->Late_Update(fTimeDelta);
	m_pPlayerLHand->Update(fTimeDelta);
}
HRESULT CPlayer::Render()
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

	m_pPlayerRHand->Render();
	m_pPlayerLHand->Render();
	return S_OK;
}
string CPlayer::Model_Animation(const vector<string>& pNames)
{
	if (!ImGui::Begin(u8"플레이어 애니메이션", NULL, ImGuiWindowFlags_MenuBar)) // 메뉴바임
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

void CPlayer::Change_Animation(PLAYER_ANIME eAnime, _bool bLoop)
{
	if (m_bOnlyActionState)
		return;

	m_bFinished = bLoop;
	m_pAnimator->Change_Animation_Enum(ETOUI(eAnime),bLoop);
	m_eAnimeState = eAnime;
}
unique_ptr<CPlayer> CPlayer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPlayer>(new CPlayer(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed BackGround");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPlayer::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPlayer>(new CPlayer(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed BackGround_Clone ");
		return pInstance;
	}
	return pInstance;
}
