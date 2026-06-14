#include "GameInstance.h"
#include "TriggerObject.h"
#include "Player_LeftHand.h"
#include "FSM_LeftHand.h"
#include "Player_Arm.h"
CPlayer_LeftHand::CPlayer_LeftHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CPlayer(pDevice, pContext)
{

}
CPlayer_LeftHand::CPlayer_LeftHand(const CPlayer_LeftHand& Prototye) : CPlayer(Prototye)
{
}
CPlayer_LeftHand::~CPlayer_LeftHand()
{
};

HRESULT CPlayer_LeftHand::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Character/Player/Left_Hand.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;

	_matrix mat = XMMatrixIdentity();
	mat = XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);
	m_pArm = static_pointer_cast<CPlayer_Arm>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"OBJ_Arm", nullptr));
	return S_OK;
}
HRESULT CPlayer_LeftHand::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CPlayer_LeftHand::Initialize(void* pArg)
{
	auto pDesc = static_cast<LEFT_HAND_DESC*>(pArg);
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 60.f;
	m_ParentsMatrix = pDesc->ParentsMatrix;
	if (FAILED(CGameObject::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;


	_matrix offset = XMMatrixIdentity();
	offset *= XMMatrixScaling(0.12f, 0.12f, 0.12f);
	offset.r[3] = XMVectorSet(2.f, -1.31f, -2.980f, 1.f);
	XMStoreFloat4x4(&m_fOffsetMatrix, offset);
	return S_OK;
}


void CPlayer_LeftHand::Hand_Pivot()
{
	auto pPlayer = m_pPlayer.lock();
	if (NULL_TRUE(pPlayer))
		return;

	_float4x4 CombinedMatrix{};
	_float4x4 BoneMatrix = pPlayer->GetAnimator()->Find_Matrix(m_iOffsetIndex);
	_matrix FinalMatrix = XMLoadFloat4x4(&BoneMatrix);

	for (uint32_t i = 0; i < 3; ++i)
		FinalMatrix.r[i] = XMVector3Normalize(FinalMatrix.r[i]);

	XMStoreFloat4x4(&CombinedMatrix, XMLoadFloat4x4(&m_fOffsetMatrix) * FinalMatrix * XMLoadFloat4x4(m_ParentsMatrix));
	m_pTransform->CombinedMatrix(&CombinedMatrix);

	if (CGameInstance::Get().Get_DIKeyState(DIK_U) & 0x80)
	{
		_float3 vLook{};
		memcpy(&vLook, m_ParentsMatrix->m[0], sizeof _float3);

		_vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), XMLoadFloat3(&vLook));
		_vector vUp = XMVector3Cross(XMLoadFloat3(&vLook), vRight);
		_vector vvLook = XMVector3Cross(vRight, vUp);
		m_pTransform->Set_State(STATE::RIGHT, vRight);
		m_pTransform->Set_State(STATE::UP, vUp);
		m_pTransform->Set_State(STATE::LOOK, vvLook);

	}
}


void	CPlayer_LeftHand::State_Move()
{
	if (!m_tagHandState.bShoot && CGameInstance::Get().Get_DIMouseOneClick(DIMK::LBUTTON, ENGINE_MOUSE::B_CLICK))
	{
		m_tagHandState.bShoot = true;
		m_pStateMachine->Change_State(FSM::HAND);
	}
}
void CPlayer_LeftHand::Priority_Update(_float fTimeDelta)
{

	Timer(fTimeDelta);
	m_pArm->Priority_Update(fTimeDelta);
}
void CPlayer_LeftHand::Update(_float fTimeDelta)
{
	Hand_Pivot();
	State_Move();
	m_pStateMachine->Update_Machine(fTimeDelta);

	m_pArm->Set_SameFlag(m_iStateFlag);
	m_pArm->Update(fTimeDelta);


	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(CPlayer_LeftHand));

}
void CPlayer_LeftHand::Late_Update(_float fTimeDelta)
{

	m_pArm->Late_Update(fTimeDelta);
}
HRESULT CPlayer_LeftHand::Render()
{
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", CGameInstance::Get().Get_Transform(D3DTS::VIEW));
	m_pShaderCom->Bind_Matrix("g_Projection", CGameInstance::Get().Get_Transform(D3DTS::PROJ));
	m_pAnimator->Bind_Resource_BoneMatrix(m_pShaderCom.get(), "g_Bone");
	Bind_ResourceFromFlag(m_pShaderCom.get(), "g_Color");
	for (auto iter : m_pMeshList)
	{
		iter->Bind_ResourceSRV(m_pShaderCom.get(), "g_Diffuse", aiTextureType_DIFFUSE, 0);
		m_pShaderCom->Begin(0);
		iter->Bind_Resource();
		iter->Render();

	}
	return S_OK;
}
void CPlayer_LeftHand::Connet_Player(shared_ptr<CGameObject> pPlayer, FSM HAND, shared_ptr<CFSM_Machine> pFsmMachine, shared_ptr<CFSM_LeftHand> pState, int32_t iKey)
{
	m_pPlayer = static_pointer_cast<CPlayer>(pPlayer);
	m_iOffsetIndex = iKey;
	pState->Set_LeftHand(SHARED_THIS(CPlayer_LeftHand), m_pArm);
	m_pStateMachine = pFsmMachine;
	m_pStateMachine->Set_Owner(pPlayer);
	m_pStateMachine->Add_State(HAND, pState);

}
void CPlayer_LeftHand::Bind_ResourceFromFlag(CShader* pShader, const _char* pConstantName)
{
	_float4 fColor{ 0,0,0,1 };
	if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
		fColor = { 0,1,0,1, };
	else if (Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_LONG)))
		fColor = { 0,1,0,1 };
	else
		fColor = { 1,1,1,1, };

	pShader->Bind_RawValue(pConstantName, &fColor, sizeof _float4);

}
unique_ptr<CPlayer_LeftHand> CPlayer_LeftHand::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPlayer_LeftHand>(new CPlayer_LeftHand(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CPlayer_LeftHand");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPlayer_LeftHand::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPlayer_LeftHand>(new CPlayer_LeftHand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CPlayer_LeftHand ");
		return pInstance;
	}
	return pInstance;
}
