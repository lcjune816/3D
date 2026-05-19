#include "FSM_LeftHand.h"
#include "Player_LeftHand.h"
#include "GameInstance.h"
#include "Player_Arm.h"
#include "TriggerObject.h"
CFSM_LeftHand::CFSM_LeftHand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_LeftHand::CFSM_LeftHand(const CFSM_LeftHand& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_LeftHand::~CFSM_LeftHand()
{
}


HRESULT CFSM_LeftHand::Initialize(void* pArg)
{
	auto desc = static_cast<FSM_PLAYER_DESC*>(pArg);

	m_StartMatrix = desc->ParentsMatrix;
	return S_OK;
}
void CFSM_LeftHand::Enter_State()
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player)) return;

	Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_L, true);
	Player->Get_AnimeState().bRHand = false;
	Player->Set_ActionState(true);

	m_fShootMaxTime = 30.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_bReFinished = false;
	m_fSpeed = 100.f;
	m_fBackShootTime = 0.f;
	m_fBackShootTick = 0.f;
	m_fForce = {};
	m_iEdgeCnt = 0;
	m_fTimerTick = 0;
	m_fTimerTime = 10.f;
	XMStoreFloat3(&m_fFirstLook, Player->Get_Transform().lock()->Get_State(STATE::LOOK));
	m_fOffset = { 0.f,0.f,0.f };
	_float4x4 matrix = *m_StartMatrix;
	memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);

	m_pArm.lock()->Get_ArmMatrix().Matrix.resize(800);

	Set_Flag(ETOUI(FSM_HAND_FLAG::SHOT), FLAGVALUE::ENABLE);
}

void CFSM_LeftHand::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pHand = m_pHand.lock();
	auto pArm = m_pArm.lock();
	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();

	Timer(fTimeDelta);

	if (!Flag_Check(ETOUI(FSM_HAND_FLAG::PULL)) && (Flag_Check(ETOUI(FSM_HAND_FLAG::SHOT)) || Flag_Check(ETOUI(FSM_HAND_FLAG::ATTACHED))))
	{
		m_fShootTimeTick += fTimeDelta;
		if (m_fShootTimeTick > 0.05f)
		{
			++m_fShootTime;
			m_fShootTimeTick = 0.f;
		}
		_matrix matrix = XMLoadFloat4x4(m_StartMatrix);
		_vector StartPos = matrix.r[3];

		if (!Flag_Check(ETOUI(FSM_HAND_FLAG::ATTACHED)))
		{
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기
		}
		

		Shoot_Hand(StartPos, Player, fTimeDelta, pArm.get(), pHand.get()); ////손 늘리기//////


		if (!Flag_Check(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE)) && (m_fShootTime >= m_fShootMaxTime))
			Hand_State_Chand(CHANGE_STATE::PULL);
		else
			pHand->Get_Transform().lock()->Set_State(STATE::POS, XMVectorSetW(XMLoadFloat3(&m_fLastHandPos), 1.f));
		///////충돌/////////
	}

	if (!m_bReFinished && Flag_Check(ETOUI(FSM_HAND_FLAG::PULL)))
	{
		_matrix matrix = XMLoadFloat4x4(m_StartMatrix);
		_vector StartPos = matrix.r[3];

		_vector Look{};  // a - (b - a ) * t 
		_float  StartLen{}, LastLen{};

		if (m_EdgePoses.empty())
		{
			Look = XMVector3Normalize(StartPos - XMLoadFloat3(&m_fLastHandPos));
			StartLen = XMVectorGetX(XMVector3Length(StartPos));
			LastLen = XMVectorGetX(XMVector3Length((StartPos - XMLoadFloat3(&m_fLastHandPos))));

		}
		else
		{
			Look = XMVector3Normalize((XMLoadFloat3(&m_EdgePoses.front().fPos) - XMLoadFloat3(&m_fLastHandPos)));
			StartLen = XMVectorGetX(XMVector3Length(StartPos));
			StartLen = XMVectorGetX(XMVector3Length((XMLoadFloat3(&m_EdgePoses.front().fPos) - XMLoadFloat3(&m_fLastHandPos))));
		}

		_float Length = LastLen * LastLen * 0.5f;
		Length += m_fSpeed;
		if (Length > 300)
			Length = 300.f;
		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * Length * fTimeDelta);

		//위치 줄이기
		Shoot_Hand(StartPos, Player, fTimeDelta, pArm.get(), pHand.get(), Flag_Check(ETOUI(FSM_HAND_FLAG::PULL) | ETOUI(FSM_HAND_FLAG::ALL_STOP)));


		pHand->Get_Transform().lock()->Set_State(STATE::POS, XMVectorSetW(XMLoadFloat3(&m_fLastHandPos), 1.f));

		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - StartPos));
		if (fDis < 0.9f * 0.9f)
		{
			pArm->Get_ArmMatrix().Matrix.clear();
			pArm->Get_ArmMatrix().CollisionIndex.clear();

			Set_Flag(ETOUI(FSM_HAND_FLAG::PULLEND), FLAGVALUE::ENABLE);
			Set_Flag(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE), FLAGVALUE::DISABLE);

		}
	}

	if ((Flag_Check(ETOUI(FSM_HAND_FLAG::ATTACHED)) || Flag_Check(ETOUI(FSM_HAND_FLAG::SHOT))) && CGameInstance::Get().Get_DIMouseOneClick(DIMK::LBUTTON, ENGINE_MOUSE::A_CLICK))
	{
		uint32_t iFlag = ETOUI(PLAYER_FLAG::ELECTRIC_LONG);
		if (pHand->Flag_Check(iFlag))
		{
			pHand->Set_Flag(iFlag, FLAGVALUE::DISABLE);
			pArm->Set_Flag(iFlag, FLAGVALUE::DISABLE);

		}
		Hand_State_Chand(CHANGE_STATE::PULL);
	}

	Hand_Collision_Check(pHand, pArm, fTimeDelta);
	Hand_End(Player.get());

}

void CFSM_LeftHand::Exit_State()
{
	Set_Flag(ETOUI(FSM_HAND_FLAG::END), FLAGVALUE::RESET);

	auto pObj = m_pHand.lock();
	if (NULL_TRUE(pObj))
		return;
	if (pObj->Get_HandState().bHandAttached)
		pObj->Get_HandState().bHandAttached = false;

	pObj->Get_HandState().bShoot = false;
	pObj->Hand_Pivot();


}

void CFSM_LeftHand::Set_LeftHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm)
{
	m_pHand = static_pointer_cast<CPlayer_LeftHand>(pObj);
	m_pArm = static_pointer_cast<CPlayer_Arm>(pArm);
}

void CFSM_LeftHand::Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer, const _float& fTimeDelta, CPlayer_Arm* PlayerArm, CPlayer_LeftHand* pRHand, _bool bFinished)
{
	//일단 레이를 쏴서 오브젝트 모서리 충돌을 확인
	//모서리에 닿으면 해당 지점을 저장
	//해당 지점으로부터 플레이어의 팔이 꺾이게 즉 플레이어의 start지점 방향으로 꺾는거고
	//
	auto& ArmMatrix = PlayerArm->Get_ArmMatrix();
	_float3 Max = pRHand->Get_Transform().lock()->Get_Max();
	_float3 Min = pRHand->Get_Transform().lock()->Get_Min();
	_float MeshLocalScaleX = (Max.x + Min.x) * 0.5f - Max.x;
	_float MeshLocalScaleY = (Max.y + Min.y) * 0.5f - Max.y;
	_float MeshLocalScaleZ = (Max.z + Min.z) * 0.5f - Max.z;

	vector<_vector> path;
	vector<_float> vLen;
	_float			total{};
	//ray 시작 위치 정하기 처음 위치로 고정
	//StatRayPos = XMVectorSetW(StatRayPos, 1.f);
	int32_t iCheck{ -1 }, sizeCnt{};
	_float	LenCheck{};
	list<_float> LenCheckList;
	_bool bCollisionDeleteCheck = {};
	uint32_t iCurrentCnt = 0;
	uint32_t iNextCnt = 0;

	ArmMatrix.CollisionIndex.clear(); //충돌지점 인덱스 초기화
	if (Flag_Check(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE))) //오브젝트에 고정 될 경우만 기둥과 충돌한다는거임
	{
		_float4x4 offsetMatrix;
		_vector emdPos{};

		emdPos = m_EdgePoses.empty() ? XMLoadFloat3(&m_fLastHandPos) : XMLoadFloat3(&m_EdgePoses.back().fPos);

		if (!m_EdgePoses.empty())
		{
			for (size_t i = 0; i < m_EdgePoses.size(); ++i)
			{
				if (m_EdgePoses[i].bCheck) //처음에 충돌나서 담은거
					++iCurrentCnt;
			}
			bCollisionDeleteCheck = m_EdgePoses.front().bCheck;

		}
		auto pObj = CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject",
			XMLoadFloat3(&m_fLastHandPos), startPos, emdPos, pPlayer->Get_Transform().lock()->Get_World(), m_EdgePoses, bFinished).lock();
		if (!m_EdgePoses.empty())
		{
			for (size_t i = 0; i < m_EdgePoses.size(); ++i)
			{
				if (m_EdgePoses[i].bCheck) //이후에 충돌나서 담은거
					++iNextCnt;
			}
		}

		if (iCurrentCnt - iNextCnt >= 0)//혹시 삭제된거 있으면 삭제하라.
		{
			for (uint32_t i = 0; i < iCurrentCnt - iNextCnt; ++i)
			{
				if (bFinished)
					m_iSizeCnt.erase(m_iSizeCnt.begin());
				else
					m_iSizeCnt.pop_back();
			}
		}

		if (NULL_FALSE(pObj))
		{
			m_EdgePoses.back().bCheck = false;
			if (static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerPtr()->Check_Trigger_Event(TRIGGER_EVENT::ELECTRICPOLE))
			{
				m_EdgePoses.back().bCheck = true;
				CGameInstance::Get().Add_Check_Collision(COLLISION::TRIGGER, pObj);
				m_iSizeCnt.push_back(m_EdgePoses.size() - 1);
				iCheck = 1;

			}
		}




	}

	if (!m_EdgePoses.empty() && bFinished) //돌아올떄
	{
		path.push_back(XMLoadFloat3(&m_fLastHandPos)); //시작위치 꺽인 위치 마지막 위치 를 순차적으로 담고
		for (auto& edge : m_EdgePoses) path.push_back(XMVectorSetW(XMLoadFloat3(&edge.fPos), 1.f));
		path.push_back(startPos);
	}
	else if (!m_EdgePoses.empty() && !bFinished)
	{
		path.push_back(XMLoadFloat3(&m_fLastHandPos)); //시작위치 꺽인 위치 마지막 위치 를 순차적으로 담고
		for (auto& edge : m_EdgePoses) path.push_back(XMVectorSetW(XMLoadFloat3(&edge.fPos), 1.f));
		path.push_back(startPos);
	}
	else
	{ //처음 담을때
		path.push_back(startPos); //시작위치 꺽인 위치 마지막 위치 를 순차적으로 담고
		for (auto& edge : m_EdgePoses) path.push_back(XMVectorSetW(XMLoadFloat3(&edge.fPos), 1.f));
		path.push_back(XMLoadFloat3(&m_fLastHandPos));
	}

	int32_t iCntCheck{ 0 };
	for (size_t j = 0; j < path.size() - 1; ++j)
	{
		//위에 서 담은거 거리 구해서 담기
		_float len = XMVectorGetX(XMVector3Length(path[j + 1] - path[j]));

		vLen.push_back(max(len, 0.0001f));

		total += len; //총 거리

		while (true)
		{
			if (m_iSizeCnt.empty() || m_iSizeCnt.size() <= iCntCheck)
				break;

			if (j >= m_iSizeCnt[iCntCheck])
			{
				if (m_iSizeCnt.size() <= iCntCheck)
					break;

				LenCheckList.push_back(total);
				++iCntCheck;
			}
			else
				break;
		}



	}
	for (size_t i = 0; i < ArmMatrix.Matrix.size(); ++i)
	{
		//와이어 비율 0 ~ 1
		_float fDiv = ArmMatrix.Matrix.size();
		_float t = min(1.f, (_float)i / (fDiv)); // 현재위치       
		_float t2 = min(1.f, (_float)(i + 1.f) / (fDiv)); //다음 위치
		_float targetDist = min(t * total, total - 0.0001f); //마디가 위치할 절대 거리
		_float NextTargetDIst = min(t2 * total, total);

		_float ft = {}, fScaleX{}, fScaleY{}, fScaleZ{};
		_vector LerpPos{}, LerpEnd{}, LerpFinal{}, vRight{}, vUp = { 0,1,0 }, vLook{};

		for (size_t j = 0; j < vLen.size(); ++j)
		{//어느 위치에 있는지 구하기
			//구간 내에서 0 ~ 1 구하기

			if (targetDist <= ft + vLen[j] || j == vLen.size() - 1)
			{
				_float LocalT = min(1.f, (targetDist - ft) / vLen[j]);
				//이구간의 시작이랑 끝으로 보간
				LerpPos = XMVectorLerp(path[j], path[j + 1], LocalT);

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

				LerpEnd = XMVectorLerp(path[k], path[k + 1], LocalT2);
				if (bFinished)
					vLook = XMVector3Normalize((LerpEnd - LerpPos));
				else
					vLook = XMVector3Normalize((LerpPos - LerpEnd));

				break;
			}
			ft += vLen[k];
		}
		ft = 0.f;

		_float fScale = XMVectorGetX(XMVector3Length(LerpEnd - LerpPos)) / MeshLocalScaleZ;
		if (fabsf(XMVectorGetY(vLook)) > 0.99f) vUp = { 1.f, 0.f, 0.f };

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		_float z = (fabsf(XMVectorGetZ(LerpEnd)) - fabsf(XMVectorGetZ(LerpPos))) / MeshLocalScaleZ;
		_vector Rot[3] = {};
		//라 업 룩 라 업

		_matrix matrix{}, S = XMMatrixIdentity(), R = XMMatrixIdentity(), T = XMMatrixIdentity();

		//           LerpPos += vLook;
		S = XMMatrixScaling(0.1f, 0.1f, fScale);
		R.r[0] = XMVectorSetW(vRight, 0.f);
		R.r[1] = XMVectorSetW(vUp, 0.f);
		R.r[2] = XMVectorSetW(vLook, 0.f);
		R.r[3] = XMVectorSet(0, 0, 0, 1.f);
		T = XMMatrixTranslationFromVector(LerpPos);

		matrix = S * R * T;
		XMStoreFloat4x4(&ArmMatrix.Matrix[i], matrix);


		if (!LenCheckList.empty())
		{
			if ((targetDist >= LenCheckList.front()))
			{

				ArmMatrix.CollisionIndex.push_back(i);
				LenCheckList.erase(LenCheckList.begin());
			}

		}

	}


}


void CFSM_LeftHand::Hand_End(CPlayer* Player)
{
	if (!m_bReFinished && Flag_Check(ETOUI(FSM_HAND_FLAG::PULLEND)))
	{
		m_EdgePoses.clear();
		m_bReFinished = true;
		Player->Set_ActionState(false);
		Player->Change_Animation(PLAYER_ANIME::SHOOTIN_L, false);
		Player->Set_ActionState(true);
	}

	if (m_bReFinished && (Player->Get_Finished() || CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80))
	{
		Player->Get_AnimeState().bRHand = false;
		Player->Set_ActionState(false);

		Player->Change_Animation(PLAYER_ANIME::IDLE, true);
		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine)) return;

		pMachine->Change_State(FSM::END);

	}
}

void CFSM_LeftHand::Hand_Collision_Check(shared_ptr<CPlayer_LeftHand> pObj, shared_ptr<CPlayer_Arm> pArm, const _float& fTimeDelta)
{
	uint32_t iFlag = ETOUI(FSM_HAND_FLAG::PULL) | ETOUI(FSM_HAND_FLAG::TIMER) | ETOUI(FSM_HAND_FLAG::ATTACHED);
	if (Flag_Check(iFlag))
		return;

	auto pTransform = pObj->Get_Transform().lock();
	CGameObject* pOb = nullptr;

	if (NULL_FALSE(pOb = CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject", m_pHand)))
	{
		CTriggerObject* Trigger = static_cast<CTriggerObject*>(pOb);
		CTrigger* pTri = Trigger->Get_TriggerPtr();

		pTri->Set_DstTransform(pTransform); 

		if (Hand_Trigger_Event(pObj, Trigger, pTri->Get_Event(), pTransform.get(), fTimeDelta))
			Trigger->Set_Trigger();
		else
			Hand_State_Chand(CHANGE_STATE::PULL);

	}
	else if (NULL_FALSE(CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_WorldObject", m_pHand)))
		Set_Flag(ETOUI(FSM_HAND_FLAG::PULL), FLAGVALUE::ENABLE);
}

_bool CFSM_LeftHand::Hand_Trigger_Event(shared_ptr<CPlayer_LeftHand> pObj, CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger, CTransform* pTransform, const _float& fTimeDelta)
{
	_vector Pos{};
	_float4x4 mat{};
	uint32_t iFlag{};
	if (pTrigger->Get_TriggerPtr()->Get_FlagState(ETOUI(TRIGGER_FLAG::CANCLE)))
	{
		Hand_State_Chand(CHANGE_STATE::PULL);
		return true;
	}

	switch (eTrigger)
	{
	case TRIGGER_EVENT::GELECTRIC:
		break;
	case TRIGGER_EVENT::BELECTRIC:
		Hand_State_Chand(CHANGE_STATE::ATTACHED_LONG);

		iFlag = ETOUI(PLAYER_FLAG::ELECTRIC_LONG);
		pObj->Set_Flag(iFlag, FLAGVALUE::ENABLE);

		pTrigger->Get_TriggerPtr()->offsetMatrix(&mat);
		memcpy(&Pos, mat.m[3], sizeof _float4);

		XMStoreFloat3(&m_fLastHandPos, Pos);
		pTransform->Set_State(STATE::POS, Pos);
		//고정은 블루
		break;
	case TRIGGER_EVENT::BATTERY:
		break;
	case TRIGGER_EVENT::ELECTRICPOLE:
		if (!Flag_Check(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE)))
			return false;

		break;
	}

	return true;
}

void CFSM_LeftHand::Hand_State_Chand(CHANGE_STATE eChange)
{
	//WALLCOLLIDE 기둥 충돌할때만 이거는 핸드 회수 후 비활성화
	//bCollect 당길떄만
	uint32_t iFlag = {};
	switch (eChange)
	{
	case CHANGE_STATE::ATTACHED_LONG:
		Set_Flag(ETOUI(FSM_HAND_FLAG::SHOT), FLAGVALUE::DISABLE);
		iFlag = ETOUI(FSM_HAND_FLAG::ATTACHED) | ETOUI(FSM_HAND_FLAG::WALLCOLLIDE);

		break;
	case CHANGE_STATE::ATTACHED_SHORT:
		Set_Flag(ETOUI(FSM_HAND_FLAG::SHOT), FLAGVALUE::DISABLE);

		iFlag = ETOUI(FSM_HAND_FLAG::ATTACHED) | ETOUI(FSM_HAND_FLAG::TIMER);
		break;
	case CHANGE_STATE::PULL:
		Set_Flag(ETOUI(FSM_HAND_FLAG::ATTACHED), FLAGVALUE::DISABLE);

		iFlag = ETOUI(FSM_HAND_FLAG::PULL);
		break;
	case CHANGE_STATE::END:
		Set_Flag(ETOUI(FSM_HAND_FLAG::ATTACHED), FLAGVALUE::DISABLE);


		iFlag = ETOUI(FSM_HAND_FLAG::PULL);
		break;
	}

	Set_Flag(iFlag, FLAGVALUE::ENABLE);//없으면 활성화 있으면 비활
}

unique_ptr<CFSM_LeftHand>		CFSM_LeftHand::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_LeftHand>(new CFSM_LeftHand(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_LeftHand::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_LeftHand>(new CFSM_LeftHand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}