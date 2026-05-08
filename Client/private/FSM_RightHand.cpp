#include "FSM_RightHand.h"
#include "Player_RightHand.h"
#include "GameInstance.h"
CFSM_RightHand::CFSM_RightHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_RightHand::CFSM_RightHand(const CFSM_RightHand& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_RightHand::~CFSM_RightHand()
{
}


HRESULT CFSM_RightHand::Initialize(void* pArg)
{

	return S_OK;
}

void CFSM_RightHand::Enter_State()
{
	auto Player = m_pPlayer.lock();
	Set_Bone();
	if (NULL_TRUE(Player)) return;

	m_iHandindex = Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_06");
	m_iFirstHandindex = Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Gun");
	m_iHandAttachedindex = Player->GetAnimator()->Find_Key("JNT_R_HandAttachment");

	m_bRightHand = Player->Get_AnimeState().bRHand;
	Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_R, true);
	Player->Get_AnimeState().bRHand = false;
	Player->Set_ActionState(true);
	m_pHand->Get_HandState().bShoot = true;

	m_fShootMaxTime = 30.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_bReFinished = false;
	m_bEndHand = false;
	m_bEndInHand = false;
	m_fSpeed = 30.f;

	XMStoreFloat3(&m_fFirstLook,Player->Get_Transform().lock()->Get_State(STATE::LOOK));

	_float4x4 matrix = m_pHand->Get_FirstMatrix();
	memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);
}

void CFSM_RightHand::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();
	PLAYER_HAND HandState = m_pHand->Get_PlayerHand();
	Mouse_Cal();

	if (m_pHand->Get_HandState().bHandAttached)
	{
		_float4x4 matrix = m_pHand->Get_LastMatrix();
		memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);
	}

	if (!m_bEndHand &&  (CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80 || m_pHand->Get_HandState().bHandAttached))
	{
		m_fShootTimeTick += fTimeDelta;

		if (m_fShootTimeTick > 0.05f)
		{
			++m_fShootTime;
			m_fShootTimeTick = 0.f;
		}
	
		_float4x4 matrix = m_pHand->Get_FirstMatrix();
		memcpy(&m_fStartPos, matrix.m[3], sizeof _float3);
		_vector StartPos = XMVectorSet(m_fStartPos.x, m_fStartPos.y, m_fStartPos.z,1.f);
	
		if(!m_pHand->Get_HandState().bHandAttached)
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기
		
		Shoot_Hand(StartPos, Player); ////손 늘리기//////
				
		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);

		m_pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);
		
		if (m_fShootTime >= m_fShootMaxTime)
			m_bEndHand = true;
		
		//Hand_Collision_Check(HandState);    ///////충돌/////////

		if (m_fSpeed >= 40)
			m_fSpeed = 40.f;
	}
	else m_bEndHand = true;

	if (m_pHand->Get_HandState().bHandAttached)
		m_bEndHand = false;

	if (!m_bReFinished && m_bEndHand && !m_pHand->Get_HandState().bHandAttached)
	{
		_float4x4 matrix = m_pHand->Get_FirstMatrix();
		memcpy(&m_fStartPos, matrix.m[3], sizeof _float3);

		_vector StartPos = XMVectorSet(m_fStartPos.x, m_fStartPos.y, m_fStartPos.z, 1.f);
		_vector Look = StartPos - XMLoadFloat3(&m_fLastHandPos);
		
		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed * 0.5f * fTimeDelta); //위치 줄이기
		Shoot_Hand(StartPos,   Player);


		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);
		m_pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);


		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - StartPos));
		if (fDis < 0.9f * 0.9f)
		{
			m_bEndInHand = true;
		}
	}

	Hand_End(Player.get());

}

void CFSM_RightHand::Exit_State()
{
	m_bRightHand = false;
	if(m_pHand->Get_HandState().bHandAttached)
		m_pHand->Get_HandState().bHandAttached = false;

}

void CFSM_RightHand::Set_RightHand(shared_ptr<CGameObject> pObj)
{
	m_pHand = static_pointer_cast<CPLayer_RightHand>(pObj);
}

void CFSM_RightHand::Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer)
{
	_matrix Inverse = pPlayer->Get_Transform().lock()->Get_World();
	Inverse = XMMatrixInverse(nullptr, Inverse); //뼈 위치를 플레이어 기준 위치로 돌리기 위함

	_float4x4 startMatrix		= m_pHand->Get_FirstMatrix();
	_matrix   EndMatrix			= pPlayer->GetAnimator()->Find_ChangeBone(m_ShootBone.back());
	_float4x4 BoneParentsMatrix = pPlayer->GetAnimator()->Find_Matrix(m_iHandAttachedindex);

	for (size_t i = 0; i < m_ShootBone.size(); ++i)
	{
		_float t = (_float)i / (_float)(m_ShootBone.size() - 1.f);
		_float t2 = (_float)(i + 1) / (_float)(m_ShootBone.size() - 2.f);
		_float3 LerpPos{}, fRight{}, fUp = { 0,1,0 };
		_float3 LerpEnd{};
		_matrix matStart = XMMatrixIdentity(), matEnd = XMMatrixIdentity();

		XMStoreFloat3(&LerpPos, XMVectorLerp(startPos, XMLoadFloat3(&m_fLastHandPos), t));
		XMStoreFloat3(&LerpPos, XMVector3TransformCoord(XMLoadFloat3(&LerpPos), Inverse));
		
		XMStoreFloat3(&LerpEnd, XMVectorLerp(startPos, XMLoadFloat3(&m_fLastHandPos), t2));
		XMStoreFloat3(&LerpEnd, XMVector3TransformCoord(XMLoadFloat3(&LerpEnd), Inverse));

		matStart.r[3] = XMLoadFloat3(&LerpPos);
		matEnd.r[3]   = XMLoadFloat3(&LerpEnd);

		CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject",
			XMLoadFloat4x4(&BoneParentsMatrix), matStart, matEnd, pPlayer->Get_Transform().lock()->Get_World(), m_EdgePoses);

		_float4x4 matrix = pPlayer->GetAnimator()->Find_Matrix(m_ShootBone[i]);
		memcpy(matrix.m[3], &LerpPos, sizeof _float3);
		pPlayer->GetAnimator()->Change_Final_BoneMatices(m_ShootBone[i], matrix);
	}

	if (m_EdgePoses.size() >= 1) 
	{
		int32_t iCnt{ 0 };
			for (size_t i = 0; i < m_ShootBone.size(); ++i)
			{
				for (size_t j = 0; j < m_EdgePoses.size(); ++j)
				{
					_float t = (_float)(i + j) / (_float)(m_EdgePoses.size() + m_ShootBone.size() - 2.f);

					_matrix ChangeBone = pPlayer->GetAnimator()->Find_ChangeBone(m_ShootBone[i]);
					_vector ChangeLerpPos = ChangeBone.r[3];
					_float3 LerpPos{};
					_vector vLook{}, vUp{ 0,1,0 }, vRight{}, vPos{};
					ChangeLerpPos = XMVector3TransformCoord(ChangeLerpPos, pPlayer->Get_Transform().lock()->Get_World());

					if (XMVectorGetX(XMVector3Length(startPos - ChangeLerpPos)) < XMVectorGetX(XMVector3Length(startPos - XMLoadFloat3(&m_EdgePoses[j]))))
					{

						XMStoreFloat3(&LerpPos, XMVectorLerp(startPos, XMLoadFloat3(&m_EdgePoses[j]), t));
						XMStoreFloat3(&LerpPos, XMVector3TransformCoord(XMLoadFloat3(&LerpPos), Inverse));

						vPos = XMVectorSetW(XMLoadFloat3(&LerpPos), 1.f);
						vLook = XMVector3Normalize(ChangeLerpPos - vPos);
						vRight = XMVector3Cross(vLook, vUp);
						vUp = XMVector3Cross(vRight, vLook);
						//memcpy(&ChangeBone.r[0], reinterpret_cast<_float*>(&vRight), sizeof _float3);
						//memcpy(&ChangeBone.r[1], reinterpret_cast<_float*>(&vUp), sizeof _float3);
						//memcpy(&ChangeBone.r[2], reinterpret_cast<_float*>(&vLook), sizeof _float3);
						memcpy(&ChangeBone.r[3], reinterpret_cast<_float*>(&vPos), sizeof _float3);
						_float4x4 matrix{};

						XMStoreFloat4x4(&matrix, ChangeBone);
						pPlayer->GetAnimator()->Change_Final_BoneMatices(m_ShootBone[i], matrix);
					}
					else
					{

						XMStoreFloat3(&LerpPos, XMVectorLerp(XMLoadFloat3(&m_EdgePoses[j]), XMLoadFloat3(&m_fLastHandPos), t));
						XMStoreFloat3(&LerpPos, XMVector3TransformCoord(XMLoadFloat3(&LerpPos), Inverse));

						vPos = XMVectorSetW(XMLoadFloat3(&LerpPos), 1.f);
						memcpy(&ChangeBone.r[3], reinterpret_cast<_float*>(&vPos), sizeof _float3);
						_float4x4 matrix{};

						XMStoreFloat4x4(&matrix, ChangeBone);

						pPlayer->GetAnimator()->Change_Final_BoneMatices(m_ShootBone[i], matrix);
					}
				}
		
			}
		
	}
	m_EdgePoses.clear();
}

void CFSM_RightHand::Mouse_Cal()
{
	POINT pt{};
	GetCursorPos(&pt);
	_float4x4 CameProj = *CGameInstance::Get().Get_Transform_Inverse(D3DTS::PROJ);
	_float4x4 CamView = *CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW);
	_float2   ViewSize = CGameInstance::Get().Get_ViewportSize();


	_float rayX = (2.f * pt.x / ViewSize.x - 1.f) / CameProj(0, 0);
	_float rayY = (-2.f * pt.y / ViewSize.y + 1.f) / CameProj(1, 1);

	//뷰포트에서의 광선 정의9
	_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector rayDir = XMVectorSet(rayX, rayY, 1.f, 0.f);

	//월드 좌표로 변환
	_matrix InverseView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&CamView));

	XMStoreFloat3(&m_fMouseLook, XMVector3Normalize(XMVector3TransformNormal(rayDir, InverseView)));

}



void CFSM_RightHand::Hand_End(CPlayer* Player)
{
	if (!m_bReFinished && m_bEndInHand)
	{

		m_pHand->Get_HandState().bShoot = false;
		m_bReFinished = true;
		Player->Set_ActionState(false);
		Player->Change_Animation(PLAYER_ANIME::SHOOT_IN, false);
		Player->Set_ActionState(true);
	}

	if (m_bReFinished && (Player->Get_Finished() || CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80))
	{
		Player->Get_AnimeState().bRHand = false;
		Player->Set_ActionState(false);

		Player->Change_Animation(PLAYER_ANIME::IDLE, true);
		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine)) return;

		pMachine->Change_State(FSM::END);

	}
}

void CFSM_RightHand::Hand_Collision_Check(const PLAYER_HAND eHand)
{
	switch (eHand)
	{
	case PLAYER_HAND::WALL:
		m_bEndHand = true;
		break;

	case PLAYER_HAND::TRIGGER:
		break;
	}
		
}

void CFSM_RightHand::Set_Bone()
{
	if (!m_ShootBone.empty())
		return;

	auto Player = m_pPlayer.lock();

	m_ShootBone.push_back(Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_01"));
	m_ShootBone.push_back(Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_02"));
	m_ShootBone.push_back(Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_03"));
	m_ShootBone.push_back(Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_04"));
	m_ShootBone.push_back(Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_05"));

}


unique_ptr<CFSM_RightHand>		CFSM_RightHand::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_RightHand>(new CFSM_RightHand(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_RightHand::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_RightHand>(new CFSM_RightHand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}