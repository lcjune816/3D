#include "GameInstance.h"
#include "Player.h"
#include "Camera.h"
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
	m_pPlayerRHand.reset();
};

HRESULT CPlayer::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Character/Player/PlayerGrab5.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;
	_matrix mat = XMMatrixIdentity();

	mat = XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);

	CNavigation::NAVIGATION_DESC NaviDesc;
	NaviDesc.iIndex = 27;
	if (FAILED(Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Navigation"), TEXT("Com_Navigation"), m_pNavigation, &NaviDesc)))
		return E_FAIL;

	m_pStateMachine = static_pointer_cast<CFSM_Machine>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Machine", nullptr));
	if (NULL_TRUE(m_pStateMachine)) return E_FAIL;


	
	auto Idle = static_pointer_cast<CFSM_Idle>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Idle", nullptr));
	if(NULL_TRUE(Idle)) return E_FAIL;

	auto Move = static_pointer_cast<CFSM_Move>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Move", nullptr));
	if (NULL_TRUE(Move)) return E_FAIL;

	auto Jump = static_pointer_cast<CFSM_Jump>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Jump", nullptr));
	if (NULL_TRUE(Jump)) return E_FAIL;
	
	auto Crouch = static_pointer_cast<CFSM_Crouch>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Crouch", nullptr));
	if (NULL_TRUE(Crouch)) return E_FAIL;
	


	///////////////////////////////////////손//////////////////////////////////////////////////////////////////////////////////
	CFSM_RightHand::FSM_PLAYER_DESC pFsmDesc;
	CFSM_LeftHand::FSM_PLAYER_DESC pFsmLeftDesc;

	CPLayer_RightHand::RIGHT_HAND_DESC pDesc;
	CPlayer_LeftHand::LEFT_HAND_DESC pLeftDesc;

	pDesc.ParentsMatrix = m_pTransform->Get_WorldPtr();
	pLeftDesc.ParentsMatrix = m_pTransform->Get_WorldPtr();

	auto RHand = static_pointer_cast<CPLayer_RightHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_PlayerRightHand", &pDesc));
	if (NULL_TRUE(RHand)) return E_FAIL;
	pFsmDesc.ParentsMatrix = RHand->Get_TransformPtr()->Get_WorldPtr();

	auto RightStateMachine = move(static_pointer_cast<CFSM_Machine>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Machine", nullptr)));
	if (NULL_TRUE(RightStateMachine)) return E_FAIL;

	auto FsmRightHand = move(static_pointer_cast<CFSM_RightHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_RightHand", &pFsmDesc)));
	if (NULL_TRUE(FsmRightHand)) return E_FAIL;

	///////////////////////////////////////////////////왼손////////////////////////////////////////////////////////////////////
	auto LHand = static_pointer_cast<CPlayer_LeftHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_PlayerLeftHand", &pLeftDesc));
	if (NULL_TRUE(LHand)) return E_FAIL;
	pFsmLeftDesc.ParentsMatrix = LHand->Get_TransformPtr()->Get_WorldPtr();

	auto LeftStateMachine = move(static_pointer_cast<CFSM_Machine>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_Machine", nullptr)));
	if (NULL_TRUE(LeftStateMachine)) return E_FAIL;

	auto FsmLeftHand = static_pointer_cast<CFSM_LeftHand>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"FSM_LeftHand", &pFsmLeftDesc));
	if (NULL_TRUE(FsmLeftHand)) return E_FAIL;
	///////////////////////////////////////손//////////////////////////////////////////////////////////////////////////////////

	//"JNT_R_Grabpack_Tube_06"
	//	"JNT_R_Grabpack_Gun"

	LHand->Connet_Player(SHARED_THIS(CPlayer), FSM::HAND, move(LeftStateMachine) , move(FsmLeftHand), GetAnimator()->Find_Key("JNT_L_HandAttachment_Pivot"));
	RHand->Connet_Player(SHARED_THIS(CPlayer), FSM::HAND, move(RightStateMachine), move(FsmRightHand), GetAnimator()->Find_Key("JNT_R_HandAttachment_Pivot"));
	
	m_pPlayerLHand = move(LHand);
	m_pPlayerRHand = move(RHand);


	m_pStateMachine->Set_Owner(SHARED_THIS(CPlayer));
	m_pStateMachine->Add_State(FSM::IDLE, Idle);
	m_pStateMachine->Add_State(FSM::MOVE, Move);
	m_pStateMachine->Add_State(FSM::JUMP, Jump);
	m_pStateMachine->Add_State(FSM::CROUCH, Crouch);
	m_pStateMachine->Change_State(FSM::IDLE);
	//17537
	CGameInstance::Get().Connect_Navigaion(m_pNavigation);
	
	return S_OK;

}

void CPlayer::Default_Height()
{
}

void CPlayer::State_Move()
{

}
void CPlayer::Hnad_State_Check()
{
	if (m_pPlayerLHand->Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_LONG)))
		m_pPlayerRHand->Set_Flag(ETOUI(PLAYER_FLAG::CONNECTHAND),FLAGVALUE::ENABLE);
	else
		m_pPlayerRHand->Set_Flag(ETOUI(PLAYER_FLAG::CONNECTHAND), FLAGVALUE::DISABLE);
}
HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CPlayer::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 5.f;
	desc.m_fSpeedPerSec = 50.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if(FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;
	
	CGameInstance::Get().Add_LightMtrl(m_PathName);
	//m_pTransform->Set_State(STATE::POS, XMVectorSet(10, 0, 10, 1));
	m_pTransform->Set_State(STATE::POS, XMVectorSetW(m_pNavigation->Find_CellPos(27),1.f));

	strcpy_s(m_pTagName, 32, "Player");
	return S_OK;
}
void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pPlayerRHand->Priority_Update(fTimeDelta);
}
void CPlayer::Update(_float fTimeDelta)
{
	if(CGameInstance::Get().Get_DIKeyState(DIK_T) & 0x80)
		CGameInstance::Get().Notify(WORLD_EVENT::BATTERY, {});

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(650, 680), ImGuiCond_FirstUseEver);
	//ImGuiCond_FirstUseEver 초기 값만 지정하고 이후에는 사용자에게 맡김

	string name = Model_Animation(m_pAnimator->Get_NameList());
	if (name != "")
		m_pAnimator->Change_Animation(name);

	Turn(fTimeDelta);
	State_Move();

  	m_pAnimator->Update(fTimeDelta);
	m_pStateMachine->Update_Machine(fTimeDelta);

	uint32_t iFlag = ETOUI(PLAYER_FLAG::JUMP) | ETOUI(PLAYER_FLAG::CROUCH);
	if (!Flag_Check(iFlag))
	{
		m_pTransform->Set_State(STATE::POS, m_pNavigation->SetUp_OnNavigation(m_pTransform->Get_State(STATE::POS), 20.f));
	}


	m_pPlayerLHand->Update(fTimeDelta);
	Hnad_State_Check();
	m_pPlayerRHand->Update(fTimeDelta);

	m_bFinished = m_pAnimator->Animation_End();

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::BLEND, SHARED_THIS(CPlayer));


}
void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pPlayerRHand->Late_Update(fTimeDelta);
	m_pPlayerLHand->Late_Update(fTimeDelta);
}
HRESULT CPlayer::Render()
{

//	m_pNavigation->Render();

	m_pAnimator->Bind_Resource_BoneMatrix(m_pShaderCom.get(), "g_Bone");
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));

	m_pAnimator->Bind_Resource_BoneMatrix(m_pShaderCom.get(), "g_Bone");
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

void CPlayer::Turn(const _float& fTimeDelta)
{
	int32_t	iMouseMove{};
	if (iMouseMove = CGameInstance::Get().Get_DIMouseMove(DIMM::X))
	{
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), iMouseMove * fTimeDelta * 3.f);
	}
	if (iMouseMove = CGameInstance::Get().Get_DIMouseMove(DIMM::Y))
	{
		m_pTransform->Turn(m_pTransform->Get_State(STATE::RIGHT), iMouseMove * fTimeDelta * 3.f);
	}
}

void CPlayer::Set_Flag(uint32_t eState, FLAGVALUE eValue)
{
	switch (eValue)
	{
	case FLAGVALUE::ENABLE:

		m_iStateFlag |= eState;
		break;

	case FLAGVALUE::DISABLE:

		m_iStateFlag &= ~eState;
		break;

	case FLAGVALUE::TOGGLE:

		m_iStateFlag ^= eState;
		break;

	case FLAGVALUE::RESET:

		m_iStateFlag = 0;
		m_fTimerTick = 0.f;
		break;

	}
}

_bool CPlayer::Flag_Check(uint32_t iFlag)
{
	if (m_iStateFlag & iFlag)
		return true;

	return false;
}

void CPlayer::Timer(const _float& fTimeDelta)
{

	if (!(Flag_Check(ETOUI(PLAYER_FLAG::TIMER))))
		return;

	m_fTimerTick += fTimeDelta;

	if (m_fTimerTick > 0.1f)
	{
		m_fTimerTick = 0;
		++m_fTimerCnt;
	}

	if (m_fTimerCnt > 50.f)
	{
		m_fTimerCnt = 0;
		uint32_t iFlag = ETOUI(PLAYER_FLAG::END);
		Set_Flag(iFlag,FLAGVALUE::RESET);
	}

}
void CPlayer::Change_Animation(PLAYER_ANIME eAnime, _bool bLoop, _bool bForce, _bool Blend )
{
	//if (m_bOnlyActionState)
	//	return;
	
	if(m_pAnimator->Change_Animation_Enum(ETOUI(eAnime),bLoop, bForce, Blend))
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
