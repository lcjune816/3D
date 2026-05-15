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

	m_iHandindex      = Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Tube_06");
	m_iFirstHandindex = Player->GetAnimator()->Find_Key("JNT_R_Grabpack_Gun");
	m_iHandAttachedindex = Player->GetAnimator()->Find_Key("JNT_R_HandAttachment");

	m_bRightHand = Player->Get_AnimeState().bRHand;
	Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_R, true);
	Player->Get_AnimeState().bRHand = false;
	Player->Set_ActionState(true);

	m_fShootMaxTime = 30.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_bReFinished = false;
	m_bEndHand = false;
	m_bEndInHand = false;
	m_fSpeed = 150.f;
	m_fBackShootTime = 0.f;
	m_fBackShootTick = 0.f;
	m_fForce = {};
	m_iEdgeCnt = 0;
	XMStoreFloat3(&m_fFirstLook,Player->Get_Transform().lock()->Get_State(STATE::LOOK));
	m_fOffset = { 0.f,0.f,0.f};
	_float4x4 matrix = *m_StartMatrix;
	memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);

	m_pArm.lock()->Get_ArmMatrix().Matrix.resize(800);
}

void CFSM_RightHand::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();
	auto pHand = m_pHand.lock();
	auto pArm = m_pArm.lock();
	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();

	if (m_eFSM.bHandAttached) //오브젝트에 고정을 시킬경우
	{
		//memcpy(&m_fLastHandPos, matrix.m[3], sizeof _float3);
	}
	

	if (!m_bEndHand &&  (CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80 || m_eFSM.bHandAttached))
	{
		m_fShootTimeTick += fTimeDelta;

		if (m_fShootTimeTick > 0.05f)
		{
			++m_fShootTime;
			m_fShootTimeTick = 0.f;
		}
	
		_float4x4 matrix = *m_StartMatrix;

		memcpy(&m_fStartPos, matrix.m[3], sizeof _float3);
		XMStoreFloat3(&m_fStartPos, XMLoadFloat3(&m_fStartPos));
		_vector StartPos = XMVectorSet(m_fStartPos.x, m_fStartPos.y, m_fStartPos.z, 1.f);
		//F = -kx -bv;
		//x 변위 늘어난 길이
		//k 탄성
		//b 감쇠 계수 
		//현재속도
		_float  Length = {};
		_vector NewForce = {};
		if (!m_eFSM.bHandAttached)
		{
			Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_fLastHandPos) - XMLoadFloat3(&m_fStartPos)));
		
			_float distRatio = clamp(Length / XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_fStartPos))), 0.f, 1.f);
			
			_float fSpeed = 400.f + (1000.f, - 400.f) *distRatio;
			Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_fLastHandPos) - XMLoadFloat3(&m_fStartPos)));

			NewForce = (XMVector3Length((XMLoadFloat3(&m_fLastHandPos) - XMLoadFloat3(&m_fStartPos)))) * fTimeDelta * XMLoadFloat3(&m_fFirstLook);
			NewForce *= 0.9;
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos)  + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기

		}
		
		Shoot_Hand(StartPos, Player,fTimeDelta, pArm.get(), pHand.get()); ////손 늘리기//////
				
		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);

		
		if(!m_bCollision && (m_fShootTime >= m_fShootMaxTime || Length > 300.f))
			m_bEndHand = true;
		else
			pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);
		    ///////충돌/////////
	}
	else m_bEndHand = true;

	if (m_eFSM.bHandAttached)
		m_bEndHand = false;

	if (!m_bReFinished && m_bEndHand && !m_eFSM.bHandAttached)
	{
		_float4x4 matrix = *m_StartMatrix;
		memcpy(&m_fStartPos, matrix.m[3], sizeof _float3);
		XMStoreFloat3(&m_fStartPos, XMLoadFloat3(&m_fStartPos));

		_vector StartPos{};
		_vector NewForce = {};
		_vector Look{};
		uint32_t V0 = {0};
		if (m_EdgePoses.empty())
		{
			Look = XMVector3Normalize((XMLoadFloat3(&m_fStartPos) - XMLoadFloat3(&m_fLastHandPos)));
			NewForce = Look * (XMVector3Length((XMLoadFloat3(&m_fStartPos) - XMLoadFloat3(&m_fLastHandPos)))) * 5.f * fTimeDelta;
			NewForce *= 0.9;
		
			_float distRatio = clamp(XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_fStartPos) - XMLoadFloat3(&m_fLastHandPos)))
				/ XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_fStartPos))), 0.f, 1.f);

		
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look* m_fSpeed * fTimeDelta);
			StartPos = XMLoadFloat3(&m_fStartPos);
			
		}else
		{
			Look = XMVector3Normalize((XMLoadFloat3(&m_EdgePoses.front().fPos) - XMLoadFloat3(&m_fLastHandPos)));
			NewForce = Look * (XMVector3Length((XMLoadFloat3(&m_fStartPos) - XMLoadFloat3(&m_fLastHandPos)))) * 5.f * fTimeDelta;
			NewForce *= 0.9f;


			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed *fTimeDelta);
			StartPos = XMLoadFloat3(&m_fStartPos);
		}
	
	
	 //위치 줄이기
		Shoot_Hand(StartPos, Player,fTimeDelta, pArm.get(),pHand.get(), m_eFSM.bCollect);


		_vector Pos = XMVectorSet(m_fLastHandPos.x, m_fLastHandPos.y, m_fLastHandPos.z, 1);
		pHand->Get_Transform().lock()->Set_State(STATE::POS, Pos);


		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - StartPos));
		if (fDis < 0.9f * 0.9f)
		{
			m_pArm.lock()->Get_ArmMatrix().Matrix.clear();
			m_pArm.lock()->Get_ArmMatrix().CollisionIndex.clear();
			m_bCollision = false;
			m_bEndInHand = true;
		}
	}

	if (m_eFSM.bHandAttached && (CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80))
	{
		m_eFSM.bHandAttached = false;
		m_eFSM.bCollect = true; //놓아줘라
		m_bEndHand = true;
	}
		

	Hand_Collision_Check(pHand);
	Hand_End(Player.get());

}

void CFSM_RightHand::Exit_State()
{

	m_bRightHand = false;
	auto pObj = m_pHand.lock();
	if (NULL_TRUE(pObj))
		return;
	if(pObj->Get_HandState().bHandAttached)
		pObj->Get_HandState().bHandAttached = false;
	m_eFSM.bCollect = false;
	pObj->Get_HandState().bShoot = false;
}

void CFSM_RightHand::Set_RightHand(shared_ptr<CGameObject> pObj, shared_ptr<CGameObject> pArm)
{
	m_pHand = static_pointer_cast<CPLayer_RightHand>(pObj);
	m_pArm  = static_pointer_cast<CPlayer_Arm>(pArm);
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
	uint32_t iCurrentCnt = 0;
	uint32_t iNextCnt = 0;

	ArmMatrix.CollisionIndex.clear(); //충돌지점 인덱스 초기화
	if (m_bCollision) //오브젝트에 고정 될 경우만 기둥과 충돌한다는거임
	{
		_float4x4 offsetMatrix;
		_vector emdPos{};

		emdPos = m_EdgePoses.empty() ? XMLoadFloat3(&m_fLastHandPos) : XMLoadFloat3(&m_EdgePoses.back().fPos);
		
		if (!m_EdgePoses.empty())
		{
			for (size_t i = 0; i < m_EdgePoses.size(); ++i)
			{
				if(m_EdgePoses[i].bCheck) //처음에 충돌나서 담은거
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
			 if (static_pointer_cast<CTriggerObject>(pObj)->Get_TriggerPtr()->Get_Trigger_Event() == TRIGGER_EVENT::ELECTRICPOLE)
			 {
				 m_EdgePoses.back().bCheck = true;
				 CGameInstance::Get().Add_Check_Collision(COLLISION::TRIGGER, pObj);
				 m_iSizeCnt.push_back(m_EdgePoses.size()-1);
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
			if (bFinished)
				vLook = XMVector3Normalize((LerpEnd -LerpPos));
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



void CFSM_RightHand::Hand_End(CPlayer* Player)
{
	if (!m_bReFinished && m_bEndInHand)
	{
		m_EdgePoses.clear();
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

void CFSM_RightHand::Hand_Collision_Check(shared_ptr<CPLayer_RightHand> pObj)
{
	if (m_eFSM.bHandAttached)
		return;
	auto pTransform = pObj->Get_Transform().lock();
	CGameObject* pOb = nullptr;

	if (NULL_FALSE(pOb = CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_TriggerObject", m_pHand)))
	{
		CTriggerObject* Trigger = static_cast<CTriggerObject*>(pOb);

		Trigger->Get_TriggerPtr()->Set_DstTransform(pTransform); //

		Hand_Trigger_Event(Trigger, Trigger->Get_TriggerPtr()->Get_Trigger_Event(), pTransform.get());
		Trigger->Set_Trigger();

	}
	else if (NULL_FALSE(CGameInstance::Get().AABB_CheckinLayer(ETOUI(LEVEL::END), L"Layer_WorldObject", m_pHand)))
		m_bEndHand = true;
}

void CFSM_RightHand::Hand_Trigger_Event(CTriggerObject* pTrigger, TRIGGER_EVENT eTrigger, CTransform* pTransform)
{
	_vector Pos{};
	_float4x4 mat{};
	switch (eTrigger)
	{
	case TRIGGER_EVENT::ELECTRIC:

		//잡는상태 
		if (!m_eFSM.bHandAttached && !m_eFSM.bCollect)
		{
			m_eFSM.bHandAttached = pTrigger->Get_TriggerPtr()->Get_OtherTrigger(); //이거 참이면 붙잡게 해놓기
			m_bCollision = m_eFSM.bHandAttached;
			pTrigger->Get_TriggerPtr()->offsetMatrix(&mat);
			//행렬 피벗 
			
			//마지막 위치 갱신 해놓기이이이이이이ㅣ이이잉아아아아ㅏ앙ㄱ아ㅏㄱ앙각아악악아
			memcpy(&Pos, mat.m[3], sizeof _float4);
			XMStoreFloat3(&m_fLastHandPos, Pos);
			pTransform->Set_State(STATE::POS, Pos);
		}
		break;
	case TRIGGER_EVENT::DOOR:
		m_eFSM.bHandAttached = false; //문이랑 충돌하면 손 놔라
		m_bEndHand = true;
		break;
	case TRIGGER_EVENT::BATTERY:
		break;

	}

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