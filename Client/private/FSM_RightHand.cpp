#include "FSM_RightHand.h"
#include "Player_RightHand.h"
#include "Player_Arm.h"
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
	m_fSpeed = 300.f;

	XMStoreFloat3(&m_fFirstLook,Player->Get_Transform().lock()->Get_State(STATE::LOOK));
	m_fOffset = { 0.f,0.1f,0.f };
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
		XMStoreFloat3(&m_fStartPos, XMLoadFloat3(&m_fStartPos) + XMLoadFloat3(&m_fOffset));
		_vector StartPos = XMVectorSet(m_fStartPos.x, m_fStartPos.y, m_fStartPos.z, 1.f);

		if(!m_pHand->Get_HandState().bHandAttached)
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기
		
		Shoot_Hand(StartPos, Player); ////손 늘리기//////
				
		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);

		m_pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);
		
		if (m_fShootTime >= m_fShootMaxTime)
			m_bEndHand = true;
		
		Hand_Collision_Check(HandState);    ///////충돌/////////

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
		XMStoreFloat3(&m_fStartPos, XMLoadFloat3(&m_fStartPos) + XMLoadFloat3(&m_fOffset));
		_vector StartPos = XMVectorSet(m_fStartPos.x, m_fStartPos.y, m_fStartPos.z, 1.f);
		_vector Look = StartPos - XMLoadFloat3(&m_fLastHandPos);
		
		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed *0.8f * fTimeDelta); //위치 줄이기
		Shoot_Hand(StartPos,   Player);


		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);
		m_pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);


		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - StartPos));
		if (fDis < 0.9f * 0.9f)
		{
			m_pArm->Get_ArmMatrix().Matrix.clear();
			m_pArm->Get_ArmMatrix().fColor.clear();
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

void CFSM_RightHand::Set_RightHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm)
{
	m_pHand = static_pointer_cast<CPLayer_RightHand>(pObj);
	m_pArm  = static_pointer_cast<CPlayer_Arm>(pArm);
}

void CFSM_RightHand::Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer)
{
	//일단 레이를 쏴서 오브젝트 모서리 충돌을 확인
	//모서리에 닿으면 해당 지점을 저장
	//해당 지점으로부터 플레이어의 팔이 꺾이게 즉 플레이어의 start지점 방향으로 꺾는거고
	//
	_float4x4 startMatrix		= m_pHand->Get_FirstMatrix();
	auto& ArmMatrix = m_pArm->Get_ArmMatrix();
	auto& ArmEdgesMatrix = m_pArm->Get_EdgePoses();

	_float3 Max = m_pHand->Get_Transform().lock()->Get_Max();
	_float3 Min = m_pHand->Get_Transform().lock()->Get_Min();
	_float MeshLocalScale = Max.z - Min.z;
	vector<_vector> path;
	vector<_float> vLen;
	_float			total{};
	//ray 시작 위치 정하기 처음 위치로 고정
	_vector StatRayPos = m_EdgePoses.empty() ? XMLoadFloat3(&m_fLastHandPos) : XMLoadFloat3(&m_EdgePoses.back());
	//StatRayPos = XMVectorSetW(StatRayPos, 1.f);
	
	if (m_pHand->Get_HandState().bHandAttached)
	{
		CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject",
			StatRayPos, startPos, StatRayPos, pPlayer->Get_Transform().lock()->Get_World(), m_EdgePoses);
	}
	
	if (!m_EdgePoses.empty())
	{

		path.push_back(XMLoadFloat3(&m_fLastHandPos)); //시작위치 꺽인 위치 마지막 위치 를 순차적으로 담고
		for (auto& edge : m_EdgePoses) path.push_back(XMVectorSetW(XMLoadFloat3(&edge), 1.f));
		path.push_back(startPos);

	}
	else
	{
		path.push_back(startPos); //시작위치 꺽인 위치 마지막 위치 를 순차적으로 담고
		for (auto& edge : m_EdgePoses) path.push_back(XMVectorSetW(XMLoadFloat3(&edge), 1.f));
		path.push_back(XMLoadFloat3(&m_fLastHandPos));
	}

	for (size_t j = 0; j < path.size() - 1; ++j)
	{
		//위에 서 담은거 거리 구해서 담기
		_float len = XMVectorGetX(XMVector3Length(path[j + 1] - path[j]));
		vLen.push_back(max(len,0.0001f));
		total += len; //총 거리
	}

	for (size_t i = 0; i < ArmMatrix.Matrix.size(); ++i)
	{
		//뭐냐이게 하나도모르겠네;;
		//와이어 비율 0 ~ 1
		_float fDiv= ArmMatrix.Matrix.size();
		_float t  = min(1.f,(_float)i   /  (fDiv)); // 현재위치       
		_float t2 = min(1.f, (_float)(i + 1.f) / (fDiv)); //다음 위치

		_float targetDist =     min(t  * total,total - 0.0001f); //마디가 위치할 절대 거리
		_float NextTargetDIst = min(t2 * total,total);
		_float ft = {};

		_vector LerpPos{}, LerpEnd{}, vRight{}, vUp = { 0,1,0 }, vLook{}, Pivot{};
		for (size_t j = 0; j < vLen.size(); ++j)
		{//어느 위치에 있는지 구하기
			//구간 내에서 0 ~ 1 구하기
			
			if (targetDist <= ft + vLen[j] || j == vLen.size() - 1)
			{
				_float LocalT =  min(1.f,(targetDist - ft) / vLen[j]);
				//이구간의 시작이랑 끝으로 보간
				vLook = XMVector3Normalize(path[j + 1] - path[j]);
				LerpPos = XMVectorLerp(path[j], path[j+1], LocalT);
				break;
			}
			ft += vLen[j];
		}
		ft = 0.f;
		for (size_t k = 0; k < vLen.size(); ++k)
		{//어느 위치에 있는지 구하기
			//구간 내에서 0 ~ 1 구하기
			if (NextTargetDIst <= ft + vLen[k] || k == vLen.size() - 1)
			{
				_float LocalT2 = min(1.f, (NextTargetDIst - ft) / vLen[k]);
				//이구간의 시작이랑 끝으로 보간
			//	vLook = XMVector3Normalize(path[j + 1] - path[j]);
				LerpEnd = XMVectorLerp(path[k], path[k + 1], LocalT2);

				break;
			}
			ft += vLen[k];
		}

		_float fScale = XMVectorGetX(XMVector3Length(LerpEnd - LerpPos)) / MeshLocalScale;
		_vector Rot[3] = {};
		//라 업 룩 라 업
		//vLook   = XMVector3Normalize(LerpPos - LerpEnd);

		if (fabsf(XMVectorGetY(vLook)) > 0.99f) vUp = { 1.f, 0.f, 0.f };
		vRight  = XMVector3Cross(vUp, vLook);
		vUp     = XMVector3Cross(vLook, vRight);

		_matrix matrix{}, S = XMMatrixIdentity(), R = XMMatrixIdentity(), T = XMMatrixIdentity();
		 
		LerpPos += vLook * (fScale * 0.5f );
		S = XMMatrixScaling(0.1f, 0.1f, fScale);
		R.r[0] = XMVectorSetW(vRight, 0.f);
		R.r[1] = XMVectorSetW(vUp, 0.f);
		R.r[2] = XMVectorSetW(vLook, 0.f);
		R.r[3] = XMVectorSet(0,0,0,1.f);
		T = XMMatrixTranslationFromVector(LerpPos);

		matrix =  S * R * T ;
		XMStoreFloat4x4(&ArmMatrix.Matrix[i], matrix);
	}
	
	
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
		m_EdgePoses.clear();
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
	case PLAYER_HAND::FORCE:
		m_bEndHand = true;
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