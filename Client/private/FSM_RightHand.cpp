#include "FSM_RightHand.h"
#include "Player_RightHand.h"
#include "Player_Arm.h"
#include "TriggerObject.h"
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
	auto desc = static_cast<FSM_PLAYER_DESC*>(pArg);

	m_StartMatrix = desc->ParentsMatrix;
	return S_OK;
}

void CFSM_RightHand::Enter_State()
{
	auto Player = m_pPlayer.lock();
	if (NULL_TRUE(Player)) return;

	Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_R, false);
	Player->Get_AnimeState().bRHand = false;

	m_fShootMaxTime = 30.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_fSpeed = 150.f;
	m_fBackShootTime = 0.f;
	m_fBackShootTick = 0.f;
	m_fForce = {};
	m_iEdgeCnt = 0;
	m_fTimerTick = 0; 
	m_fTimerTime = 10.f;
	m_bStop = m_bOnlyone = m_bFront = m_bReFinished = false;
	m_iMaxSpeed = 400;
	XMStoreFloat3(&m_fFirstLook,Player->Get_Transform().lock()->Get_State(STATE::LOOK));
	m_fOffset = { 0.f,0.f,0.f};
	_float4x4 matrix = *m_StartMatrix;
	memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);

	m_pArm.lock()->Get_ArmMatrix().Matrix.resize(800);
	m_fLength = 5.f;
	Set_Flag(ETOUI(FSM_HAND_FLAG::SHOT), FLAGVALUE::ENABLE);
	m_eAction = FSM_ACTION::ACTION;
}

void CFSM_RightHand::Update_State(_float fTimeDelta)
{
	auto Player = m_pPlayer.lock();
	auto pHand  = m_pHand.lock();
	auto pArm   =m_pArm.lock();
	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();

	Timer(fTimeDelta);
	switch (m_eAction)
	{
	case FSM_ACTION::IDLE:
		break;
	case FSM_ACTION::ACTION:
		Action_Hand(fTimeDelta,Player,pHand,pArm);
		break;
	case FSM_ACTION::RETURN:
		Return_Hand(fTimeDelta, Player, pHand, pArm);
		break;
	}

		
	Hand_Collision_Check(pHand, pArm ,fTimeDelta);
	Hand_End(Player.get());

}

void CFSM_RightHand::Exit_State()
{
	Set_Flag(ETOUI(FSM_HAND_FLAG::END), FLAGVALUE::RESET);

	auto pObj = m_pHand.lock();
	if (NULL_TRUE(pObj))
		return;
	if(pObj->Get_HandState().bHandAttached)
		pObj->Get_HandState().bHandAttached = false;

	pObj->Get_HandState().bShoot = false;
	pObj->Hand_Pivot();


}
void CFSM_RightHand::Set_RightHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm)
{
	m_pHand = static_pointer_cast<CPLayer_RightHand>(pObj);
	m_pArm  = static_pointer_cast<CPlayer_Arm>(pArm);
}
void CFSM_RightHand::Action_Hand(const _float& fTimeDelta, shared_ptr<CPlayer> pPlayer, shared_ptr<CPLayer_RightHand> pHand, shared_ptr<CPlayer_Arm> pArm)
{
	//F = -KX -CV
	// K 스프링 강도 X 목표 위치와 거리 C 감쇠 계수 V속도
	if (!Flag_Check(ETOUI(FSM_HAND_FLAG::ALL_STOP)) && (Flag_Check(ETOUI(FSM_HAND_FLAG::SHOT)) || Flag_Check(ETOUI(FSM_HAND_FLAG::ATTACHED))))
	{
		_matrix matrix = XMLoadFloat4x4(m_StartMatrix);
		_vector StartPos = matrix.r[3];
		_float  Length = {};
		_vector NewForce = {};
		_float t{};
		m_fSpeed += 100.f * fTimeDelta;
		
		m_fSpeed = min(m_fSpeed, 500.f);
		if (!(Flag_Check(ETOUI(FSM_HAND_FLAG::ATTACHED))) && !m_bStop)
		{
			m_fShootTimeTick += fTimeDelta;
			if (m_fShootTimeTick > 0.05f)
			{
				++m_fShootTime;
				m_fShootTimeTick = 0.f;
			}
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기

		}else XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * 100.f * fTimeDelta);

		Shoot_Hand(StartPos, pPlayer, fTimeDelta, pArm.get(), pHand.get()); ////손 늘리기//////			


		if (!Flag_Check(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE)) && (m_fShootTime >= m_fShootMaxTime)) //기둥일떄..
		{
			Hand_State_Chand(CHANGE_STATE::END);
		}
		else if (Flag_Check(ETOUI(FSM_HAND_FLAG::PAUSE)) && (m_fShootTime >= m_fShootMaxTime)) //그랩팩 발쏴 하는동안 멈추기..
		{
			Hand_State_Chand(CHANGE_STATE::PULL);
		}
		
		pHand->Get_Transform().lock()->Set_State(STATE::POS, XMVectorSetW(XMLoadFloat3(&m_fLastHandPos), 1.f));

	}

	if (CGameInstance::Get().Get_DIMouseOneClick(DIMK::RBUTTON, ENGINE_MOUSE::A_CLICK))
		Hand_State_Chand(CHANGE_STATE::END);
}
void CFSM_RightHand::Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer, const _float& fTimeDelta,  CPlayer_Arm* PlayerArm, CPLayer_RightHand* pRHand, _bool bFinished )
{
	//일단 레이를 쏴서 오브젝트 모서리 충돌을 확인
	//모서리에 닿으면 해당 지점을 저장
	//해당 지점으로부터 플레이어의 팔이 꺾이게 즉 플레이어의 start지점 방향으로 꺾는거고
	//
	auto& ArmMatrix = PlayerArm->Get_ArmMatrix();
	_float3 Max = pRHand->Get_Transform().lock()->Get_Max();
	_float3 Min = pRHand->Get_Transform().lock()->Get_Min();
	_float MeshLocalScaleX = (Max.x + Min.x) *0.5f - Max.x;
	_float MeshLocalScaleY = (Max.y + Min.y) *0.5f - Max.y;
	_float MeshLocalScaleZ = (Max.z + Min.z) *0.5f - Max.z;

	vector<_vector> path;
	vector<_float> vLen;
	_float			total{};
	//ray 시작 위치 정하기 처음 위치로 고정
	//StatRayPos = XMVectorSetW(StatRayPos, 1.f);
	int32_t iCheck{ -1 }, sizeCnt{};
	_float	LenCheck{};
	list<_float> LenCheckList;
	_bool bCollisionDeleteCheck = {};
	int32_t iCurrentCnt = 0;
	int32_t iNextCnt = 0;

	_vector vStartPos = startPos;
	ArmMatrix.CollisionIndex.clear(); //충돌지점 인덱스 초기화
	if (Flag_Check(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE))) //오브젝트에 고정 될 경우만 기둥과 충돌한다는거임
	{
		_float4x4 offsetMatrix;
		_vector vEndPos{};
		vEndPos = m_EdgePoses.empty() ? XMLoadFloat3(&m_fLastHandPos) : XMLoadFloat3(&m_EdgePoses.back().fPos);
		
		if (!m_EdgePoses.empty())
		{
			for (size_t i = 0; i < m_EdgePoses.size(); ++i)
			{
				if(m_EdgePoses[i].bCheck) //처음에 충돌나서 담은거
					++iCurrentCnt;
			}
			bCollisionDeleteCheck = m_EdgePoses.front().bCheck;

		}

		if (m_bFront && !bFinished)
		{
			vStartPos = m_EdgePoses.empty() ? startPos : XMLoadFloat3(&m_fLastHandPos);
		}
			
		if (CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject",
			XMLoadFloat3(&m_fLastHandPos), vStartPos, vEndPos, pPlayer->Get_Transform().lock()->Get_World(), m_EdgePoses, m_iSizeCnt, bFinished, m_bFront))

			iCheck = 1;
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
	}
	if (!m_EdgePoses.empty() && m_bFront && !bFinished)
	{
		path.push_back(startPos);
		for (auto& edge : m_EdgePoses)
		path.push_back(XMVectorSetW(XMLoadFloat3(&edge.fPos), 1.f));
		path.push_back(XMLoadFloat3(&m_fLastHandPos));
		//앞에서부터 

	}
	else if(!m_EdgePoses.empty() && m_bFront && bFinished)
	{

		path.push_back(XMLoadFloat3(&m_fLastHandPos));
		for (int32_t i= m_EdgePoses.size() - 1; i >= 0; --i)
		{
			path.push_back(XMVectorSetW(XMLoadFloat3(&m_EdgePoses[i].fPos), 1.f));

		}
		path.push_back(startPos);

	}
	else if (!m_EdgePoses.empty()) //돌아올떄
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
	
	int32_t iCntCheck{0};
	for (size_t j = 0; j < path.size() - 1; ++j)
	{
		//위에 서 담은거 거리 구해서 담기
		_float len = XMVectorGetX(XMVector3Length( path[j+1]- path[j]));
		
		vLen.push_back(max(len,0.0001f));
		
		total += len; //총 거리
	
		while (true)
		{
			if (m_iSizeCnt.empty() || m_iSizeCnt.size() <= iCntCheck)
				break;

			if (j >= m_iSizeCnt[iCntCheck])
			{
				if (m_iSizeCnt.size()<= iCntCheck)
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
		_float fDiv= ArmMatrix.Matrix.size();
		_float t  = min(1.f,(_float)i   /  (fDiv)); // 현재위치       
		_float t2 = min(1.f, (_float)(i + 1.f) / (fDiv)); //다음 위치
		_float targetDist =     min(t  * total,total - 0.0001f); //마디가 위치할 절대 거리
		_float NextTargetDIst = min(t2 * total,total);
	
		_float ft = {}, fScaleX{}, fScaleY{}, fScaleZ{};
		_vector LerpPos{}, LerpEnd{}, LerpFinal{}, vRight{}, vUp = { 0,1,0 }, vLook{};
		
		for (size_t j = 0; j < vLen.size(); ++j)
		{//어느 위치에 있는지 구하기
			//구간 내에서 0 ~ 1 구하기
			
			if (targetDist <= ft + vLen[j] || j == vLen.size() - 1)
			{
				_float LocalT =  min(1.f,(targetDist - ft) / vLen[j]);
				//이구간의 시작이랑 끝으로 보간
					LerpPos = XMVectorLerp(path[j] , path[j + 1], LocalT);
				
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
			
				if (!bFinished || m_bFront)
					vLook = XMVector3Normalize((LerpPos - LerpEnd));
					else
				vLook = XMVector3Normalize((LerpEnd - LerpPos));
				break;
			}
			ft += vLen[k];
		}
		ft = 0.f;

		_float fScale = XMVectorGetX(XMVector3Length(LerpEnd - LerpPos)) / MeshLocalScaleZ;
				if (fabsf(XMVectorGetY(vLook)) > 0.99f) vUp = { 1.f, 0.f, 0.f };
				
				vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
				vUp =    XMVector3Normalize(XMVector3Cross(vLook, vRight));

		_float z = (fabsf(XMVectorGetZ(LerpEnd)) - fabsf(XMVectorGetZ(LerpPos)))/ MeshLocalScaleZ;
		_vector Rot[3] = {};
		//라 업 룩 라 업

		_matrix matrix{}, S = XMMatrixIdentity(), R = XMMatrixIdentity(), T = XMMatrixIdentity();
		 
		//           LerpPos += vLook;
		S = XMMatrixScaling(0.1f, 0.1f, fScale);
		R.r[0] = XMVectorSetW(vRight, 0.f);
		R.r[1] = XMVectorSetW(vUp, 0.f)   ;
		R.r[2] = XMVectorSetW(vLook, 0.f) ;
		R.r[3] = XMVectorSet(0,0,0,1.f);
		T = XMMatrixTranslationFromVector(LerpPos);

		matrix = S*  R * T ;
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

void CFSM_RightHand::Return_Hand(const _float& fTimeDelta, shared_ptr<CPlayer> pPlayer, shared_ptr<CPLayer_RightHand> pHand, shared_ptr<CPlayer_Arm> pArm)
{
	_matrix matrix = XMLoadFloat4x4(m_StartMatrix);
	_vector StartPos = matrix.r[3];

	_vector Look{}, EdgePos{}, LastPos{};  // a + (b - a ) * t 
	//
	_float  StartLen{}, LastLen{};
	if (m_EdgePoses.empty())
	{
		Look = XMVector3Normalize(StartPos - XMLoadFloat3(&m_fLastHandPos));
		LastPos = StartPos;
	}
	else
	{
		if (m_bFront)
			EdgePos = XMLoadFloat3(&m_EdgePoses.back().fPos);
		else
			EdgePos = XMLoadFloat3(&m_EdgePoses.front().fPos);
		
		Look = XMVector3Normalize((EdgePos - XMLoadFloat3(&m_fLastHandPos)));
		StartLen = XMVectorGetX(XMVector3Length(StartPos));
		LastPos = EdgePos;
	}

	LastLen = XMVectorGetX(XMVector3Length((LastPos - XMLoadFloat3(&m_fLastHandPos))));
	//_float Length = LastLen * LastLen * 0.5f;

	_float fAccel = max(LastLen * 120.f, m_iMaxSpeed);
	m_fSpeed += fAccel * fTimeDelta;
	m_fSpeed *= 0.8f;
	m_fSpeed = min(m_fSpeed, 600.f);
	XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed * fTimeDelta);

	//위치 줄이기
	Shoot_Hand(StartPos, pPlayer, fTimeDelta, pArm.get(), pHand.get(), Flag_Check(ETOUI(FSM_HAND_FLAG::PULL) | ETOUI(FSM_HAND_FLAG::ALL_STOP)));


	pHand->Get_Transform().lock()->Set_State(STATE::POS, XMVectorSetW(XMLoadFloat3(&m_fLastHandPos), 1.f));


	_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - StartPos));
	if (fDis < 60.f)
	{
		m_iMaxSpeed = 200;
	}
	if (fDis < 15.f)
	{
		pArm->Get_ArmMatrix().Matrix.clear();
		pArm->Get_ArmMatrix().CollisionIndex.clear();

		pPlayer->Change_Animation(PLAYER_ANIME::SHOOTIN_R, false);
		Set_Flag(ETOUI(FSM_HAND_FLAG::PULLEND), FLAGVALUE::ENABLE);
		Set_Flag(ETOUI(FSM_HAND_FLAG::WALLCOLLIDE), FLAGVALUE::DISABLE);
		m_eAction = FSM_ACTION::IDLE;
	}
}



void CFSM_RightHand::Hand_End(CPlayer* Player)
{
	if (!m_bReFinished && Flag_Check(ETOUI(FSM_HAND_FLAG::PULLEND)))
	{
		m_EdgePoses.clear();
		m_bReFinished = true;
		Set_Flag(ETOUI(FSM_HAND_FLAG::PAUSE), FLAGVALUE::DISABLE);
	}

	if (m_bReFinished )
	{
		Player->Get_AnimeState().bRHand = false;
		Player->Set_ActionState(false);
		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine)) return;

		pMachine->Change_State(FSM::END);

	}
}

void CFSM_RightHand::Hand_Collision_Check(shared_ptr<CPLayer_RightHand> pObj, shared_ptr<CPlayer_Arm> pArm,const _float& fTimeDelta)
{
	uint32_t iFlag =  ETOUI(FSM_HAND_FLAG::TIMER) | ETOUI(FSM_HAND_FLAG::ALL_STOP);
	if (Flag_Check(iFlag) && (!Flag_Check(ETOUI(FSM_HAND_FLAG::PAUSE))))
		return;

	auto pTransform = pObj->Get_Transform().lock();
	CGameObject* pOb = nullptr;

	if (NULL_FALSE(pOb = CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject", m_pHand)))
	{
		CTriggerObject* Trigger = static_cast<CTriggerObject*>(pOb);
		CTrigger*			pTri = Trigger->Get_TriggerPtr();
		if(pObj->Flag_Check(ETOUI(PLAYER_FLAG::CONNECTHAND)) && !(pTri->Check_Trigger_Event(TRIGGER_EVENT::PANNEL)))
			return;

		pTri->Set_DstTransform(pTransform);
		if (Hand_Trigger_Event(pObj,  Trigger, pTri->Get_Event(), pTransform.get(), fTimeDelta))
			Trigger->Set_Trigger();
			
	}
	else if ( NULL_FALSE(CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_WorldObject", m_pHand)))
			Hand_State_Chand(CHANGE_STATE::END);
}

_bool CFSM_RightHand::Update_LastPos(CTriggerObject* pTrigger, CTransform* pTransform)
{
	_float4x4 mat{};
	_matrix  offsetmat;
	_vector Pos{}, vLook{};
	_float3 fScale = pTransform->Get_Scaled();
	if (!pTrigger->Get_TriggerPtr()->offsetMatrix(&mat))
		return false;

	
	offsetmat = XMLoadFloat4x4(&mat);
	
	XMStoreFloat3(&m_fFirstLook, XMVectorSet(-1,0,0,0));
	XMStoreFloat3(&m_fLastHandPos, offsetmat.r[3]);

	pTransform->Set_State(STATE::RIGHT,offsetmat.r[0] * fScale.x);
	pTransform->Set_State(STATE::UP   ,offsetmat.r[1] * fScale.y);
	pTransform->Set_State(STATE::LOOK ,offsetmat.r[2] * fScale.z);
	pTransform->Set_State(STATE::POS  ,offsetmat.r[3]);

	return true;
}


_bool CFSM_RightHand::Hand_Trigger_Event(shared_ptr<CPLayer_RightHand> pObj, CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger, CTransform* pTransform, const _float& fTimeDelta)
{
	uint32_t iFlag{};
	_bool    bCheck{false};
	//Cancle은 뭐 그냥 활성화 비활성화만
	
	if (pTrigger->Get_TriggerPtr()->Check_Flag(ETOUI(TRIGGER_FLAG::CANCLE)))
	{
		Hand_State_Chand(CHANGE_STATE::END);
		bCheck = true;
	}

	switch (eTrigger)
	{
	case TRIGGER_EVENT::GELECTRIC:
		
		if (!(pTrigger->Get_TriggerPtr()->Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER))))
		{
			//조건 1 : 플레이어 녹색전기 활성화 된 상태 일때
			//조건 2 : 전기 패널의 전기가 비활성화 된 상태
			//결과 값 : 전기 패널 활성화
			if (pObj->Flag_Check(ETOUI(PLAYER_FLAG::ELECTRIC_SHORT)))
			{
				bCheck = true;
				Hand_State_Chand(CHANGE_STATE::ATTACHED_SHORT); //잡기
			}
			else
				bCheck = false;
		}
		else
		{
			//조건 1: 전기 패널의 전기가 활성화 된 상태일때
			//조건 2 : 플레이어의 팔 전기 활성화	
			iFlag = ETOUI(PLAYER_FLAG::ELECTRIC_SHORT) | ETOUI(PLAYER_FLAG::TIMER);
			pObj->Set_Flag(iFlag, FLAGVALUE::ENABLE);
			Hand_State_Chand(CHANGE_STATE::ATTACHED_SHORT);
			bCheck = true;
		}

		Update_LastPos(pTrigger, pTransform);
		break;
	case TRIGGER_EVENT::BELECTRIC:
		bCheck = false;
		break;
	case TRIGGER_EVENT::PANNEL:
		Hand_State_Chand(CHANGE_STATE::ATTACHED_SHORT);
		bCheck = true;
		break;
	case TRIGGER_EVENT::BATTERY:
		bCheck = true;
		break;
	case TRIGGER_EVENT::ROLLUPDOOR:
		if (!pTrigger->Get_TriggerPtr()->Check_Flag(ETOUI(TRIGGER_FLAG::FTRIGGER)))
		bCheck = false;
		break;
	case TRIGGER_EVENT::ELECTRICPOLE:
		bCheck = false;
		break;
	case TRIGGER_EVENT::PUZZLEROT:
		Hand_State_Chand(CHANGE_STATE::PAUSE);

		m_bFront = true;
		if (!(Update_LastPos(pTrigger, pTransform)))
		{
			m_bStop = false;
			return false;
		}
		else
		{
			m_bStop = true;
			if(!m_bOnlyone)
				m_fShootMaxTime += 0.001f;

		}
			
		return true;
		break;
	}

	if (!bCheck)
	{
		Hand_State_Chand(CHANGE_STATE::END);
		return bCheck;
	}
		
	return true;
}

void CFSM_RightHand::Hand_State_Chand(CHANGE_STATE eChange)
{
	//WALLCOLLIDE 기둥 충돌할때만 이거는 핸드 회수 후 비활성화
	//bCollect 당길떄만
	uint32_t iFlag = {};
	switch(eChange)
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
		Set_Flag(ETOUI(FSM_HAND_FLAG::ATTACHED) | ETOUI(FSM_HAND_FLAG::SHOT) , FLAGVALUE::DISABLE);
		m_fShootTimeTick = 0.f;
		m_bOnlyone = true;
		m_eAction = FSM_ACTION::RETURN;
		iFlag = ETOUI(FSM_HAND_FLAG::PULL);
		break;
	case CHANGE_STATE::END:
		if (m_bFront)
			return;
		Set_Flag(ETOUI(FSM_HAND_FLAG::ATTACHED) | ETOUI(FSM_HAND_FLAG::SHOT) | ETOUI(FSM_HAND_FLAG::TIMER), FLAGVALUE::DISABLE);
		m_eAction = FSM_ACTION::RETURN;
		iFlag = ETOUI(FSM_HAND_FLAG::ALL_STOP);
		break;
	case CHANGE_STATE::PAUSE:
		iFlag = ETOUI(FSM_HAND_FLAG::WALLCOLLIDE) | ETOUI(FSM_HAND_FLAG::PAUSE) ;
		break;
	}
	
	Set_Flag(iFlag, FLAGVALUE::ENABLE);//없으면 활성화 있으면 비활
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
		MSG_BOX("Create Failed CFSM_RightHand Clone");
		return nullptr;
	}

	return pInstance;

}