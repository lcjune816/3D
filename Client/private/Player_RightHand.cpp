#include "GameInstance.h"
#include "Player_RightHand.h"
#include "TriggerObject.h"
#include "Player.h"
CPLayer_RightHand::CPLayer_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :
	CGameObject(pDevice, pContext)
{

}
CPLayer_RightHand::CPLayer_RightHand(const CPLayer_RightHand& Prototye) : CGameObject(Prototye)
{
}
CPLayer_RightHand::~CPLayer_RightHand()
{
};

void CPLayer_RightHand::Hand_Pivot()
{
	auto pPlayer = m_pPlayer.lock();
	if (NULL_TRUE(pPlayer))
		return;

	_matrix mat = XMMatrixIdentity();
	_vector PlayerX, PlayerY, PlayerZ, PlayerW;
	PlayerX = XMVector3Normalize(pPlayer->Get_Transform().lock()->Get_State(STATE::RIGHT));
	PlayerY = XMVector3Normalize(pPlayer->Get_Transform().lock()->Get_State(STATE::UP));
	PlayerZ = XMVector3Normalize(pPlayer->Get_Transform().lock()->Get_State(STATE::LOOK));
	PlayerW = pPlayer->Get_Transform().lock()->Get_State(STATE::POS);

	memcpy(&mat.r[0], &PlayerX, sizeof _float3);
	memcpy(&mat.r[1], &PlayerY, sizeof _float3);
	memcpy(&mat.r[2], &PlayerZ, sizeof _float3);
	memcpy(&mat.r[3], &PlayerW, sizeof _float3);

	//JNT_L_HandAttachment
	_float4x4	matPos = pPlayer->GetAnimator()->Find_Matrix("JNT_R_HandAttachment");

	_vector x = XMVector3Normalize(XMVectorSet(matPos.m[0][0], matPos.m[0][1], matPos.m[0][2], 0.f));
	_vector y = XMVector3Normalize(XMVectorSet(matPos.m[1][0], matPos.m[1][1], matPos.m[1][2], 0.f));
	_vector z = XMVector3Normalize(XMVectorSet(matPos.m[2][0], matPos.m[2][1], matPos.m[2][2], 0.f));

	memcpy(&matPos.m[0], &x, sizeof _float3);
	memcpy(&matPos.m[1], &y, sizeof _float3);
	memcpy(&matPos.m[2], &z, sizeof _float3);

	//_float3 offsetRot = { 0,0,0 };
	_float3 offsetPos = { -2.f,-1.4f,-3.180f };
	_matrix matoffset = XMMatrixIdentity();

	memcpy(&matoffset.r[3], &offsetPos, sizeof _float3);

	mat = XMMatrixScaling(-0.1f, 0.1f, 0.1f) * matoffset * XMLoadFloat4x4(&matPos) * mat;

	if (!m_tagHandState.bShoot)
	{
		m_pTransform->Set_State(STATE::RIGHT, mat.r[0]);
		m_pTransform->Set_State(STATE::UP, mat.r[1]);
		m_pTransform->Set_State(STATE::LOOK, mat.r[2]);
		m_pTransform->Set_State(STATE::POS, mat.r[3]);
	}
	
	XMStoreFloat4x4(&m_StartMatrix, mat);
}

void CPLayer_RightHand::Hand_Collision()
{

	CGameObject* pObj = nullptr;

	if (NULL_FALSE(CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_WorldObject", SHARED_THIS(CPLayer_RightHand))))
		m_eRHand = PLAYER_HAND::WALL;
	else if (NULL_FALSE(pObj = CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject", SHARED_THIS(CPLayer_RightHand))))
	{
		auto Obj = static_cast<CTriggerObject*>(pObj);
		Obj->Get_TriggerPtr()->Set_DstTransform(m_pTransform.get());
		Hand_Trigger_Event(Obj, Obj->Get_TriggerPtr()->Get_Trigger_Event());
		Obj->Set_Trigger();

		m_eRHand = PLAYER_HAND::TRIGGER;
	}
	else
	{
		m_eRHand = PLAYER_HAND::END;
	}
}

void CPLayer_RightHand::Hand_Trigger_Event(CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger)
{
	switch (eTrigger)
	{
	case TRIGGER_EVENT::ELECTRIC:
			m_tagHandState.bHandAttached = pTrigger->Get_TriggerPtr()->Get_OtherTrigger();
			XMStoreFloat4x4(&m_LastMatrix,m_pTransform->Get_World());
			break;
	}
}

HRESULT CPLayer_RightHand::Ready_Component()
{

	Engine::IMPORTMODEL_DESC importModel;
	importModel.pFile = "../../Resource/Character/Player/Right_Hand.fbx";
	importModel.bAllModel = 1;
	importModel.eType = MESH_TYPE::ANIME;

	_matrix mat = XMMatrixIdentity();
	mat = XMMatrixRotationY(XMConvertToRadians(180.f));
	CGameInstance::Get().ImportModel_Anime(importModel, m_pMeshList, m_pAnimator, m_pTransform, mat);

	return S_OK;
}
HRESULT CPLayer_RightHand::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CPLayer_RightHand::Initialize(void* pArg)
{
	CTransform::TRANSFORM_DESC desc;
	desc.m_fRotationPerSec = 0.f;
	desc.m_fSpeedPerSec = 30.f;

	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_Animation"),
		TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	CGameInstance::Get().Add_LightMtrl(m_PathName);

	return S_OK;
}
void CPLayer_RightHand::Priority_Update(_float fTimeDelta)
{
	
}
void CPLayer_RightHand::Update(_float fTimeDelta)
{
	Hand_Pivot();

	Hand_Collision();

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, SHARED_THIS(CPLayer_RightHand));

}
void CPLayer_RightHand::Late_Update(_float fTimeDelta)
{

}
HRESULT CPLayer_RightHand::Render()
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
unique_ptr<CPLayer_RightHand> CPLayer_RightHand::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CPLayer_RightHand>(new CPLayer_RightHand(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CPLayer_RightHand");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CPLayer_RightHand::Clone(void* pArg)
{
	auto pInstance = shared_ptr<CPLayer_RightHand>(new CPLayer_RightHand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CPLayer_RightHand ");
		return pInstance;
	}
	return pInstance;
}
