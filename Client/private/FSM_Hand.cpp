#include "FSM_Hand.h"
#include "GameInstance.h"
CFSM_Hand::CFSM_Hand(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) :CPlayer_FSM(pDevice, pContext)
{
}

CFSM_Hand::CFSM_Hand(const CFSM_Hand& Prototype) :CPlayer_FSM(Prototype)
{
}

CFSM_Hand::~CFSM_Hand()
{
}


HRESULT CFSM_Hand::Initialize(void* pArg)
{

	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_01");
	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_02");
	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_03");
	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_04");
	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_05");
	m_ShootBone[ETOUI(ARROW::RIGHT)].push_back("JNT_R_Grabpack_Tube_06");

	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_01");
	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_02");
	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_03");
	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_04");
	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_05");
	m_ShootBone[ETOUI(ARROW::LEFT)].push_back("JNT_L_Grabpack_Tube_06");

	return S_OK;
}

void CFSM_Hand::Enter_State()
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player)) return;


	if (Player->Get_AnimeState().bTwoHand)
	{
		m_bTwoHand = Player->Get_AnimeState().bTwoHand;
		
		Player->Change_Animation(PLAYER_ANIME::SHOOT_OUT, true);
		Player->Get_AnimeState().bTwoHand = false;
	}		
	else if (Player->Get_AnimeState().bLHand)
	{
		m_bLeftHand = Player->Get_AnimeState().bLHand;
		strcpy_s(m_HandName, sizeof(m_HandName),"JNT_L_Grabpack_Tube_06") ;
		strcpy_s(m_FirstHand, sizeof(m_FirstHand), "JNT_L_Grabpack_Gun");
		strcpy_s(m_HandAttached, sizeof(m_HandAttached), "JNT_L_HandAttachment");
		m_iArrow = ETOUI(ARROW::LEFT);
		Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_R, true);
		Player->Get_AnimeState().bLHand = false;
	}
	else if (Player->Get_AnimeState().bRHand)
	{

		strcpy_s(m_HandName, sizeof(m_HandName), "JNT_R_Grabpack_Tube_06");
		strcpy_s(m_FirstHand, sizeof(m_FirstHand), "JNT_R_Grabpack_Gun");
		strcpy_s(m_HandAttached, sizeof(m_HandAttached), "JNT_R_HandAttachment");

		m_bRightHand = Player->Get_AnimeState().bRHand;
		m_iArrow = ETOUI(ARROW::RIGHT);
		Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_L, true);
		Player->Get_AnimeState().bRHand = false;

	}
	
	Player->Set_ActionState(true);
	m_fShootMaxTime = 10.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_bReFinished = false;
	m_bEndHand = false;
	m_bEndInHand = false;
	m_fSpeed = 10.f;
	for (int32_t i = 0; i < 6; ++i)
	{
		m_fLook[i] = {};
	}
	_float4x4 End = Player->GetAnimator()->Find_Matrix(m_HandName);
	memcpy(&m_fLastHandPos, End.m[3], sizeof _float3);
}

void CFSM_Hand::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();

	Mouse_Cal();

	if (!m_bEndHand && (CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80 || CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80) )
	{
		m_fShootTimeTick += fTimeDelta;

		if (m_fShootTimeTick > 0.05f)
		{
			++m_fShootTime;
			m_fShootTimeTick = 0.f;
		}
		_vector Look = Player->Get_Transform()->Get_State(STATE::LOOK);
		_float4x4 start = Player->GetAnimator()->Find_Matrix(m_FirstHand); //처음위치
		_vector startPos = XMLoadFloat4x4(&start).r[3];

		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed ); //마지막 위치 늘려서 보간하기
		Shoot_Hand(startPos, Player);

		_float4x4 HandMatrix = Player->GetAnimator()->Find_Matrix(m_HandAttached);
		memcpy(&HandMatrix.m[3], &m_fLastHandPos, sizeof _float3);
		Player->GetAnimator()->Change_Final_BoneMatices(m_HandAttached, HandMatrix);

		if (m_fShootTime >= m_fShootMaxTime)
			m_bEndHand = true;

		if (m_fSpeed >= 40)
			m_fSpeed =40.f;
	}
	else m_bEndHand = true;

	if (!m_bReFinished && m_bEndHand)
	{
		_float4x4 start = Player->GetAnimator()->Find_Matrix(m_FirstHand); //처음위치
		_vector startPos = XMLoadFloat4x4(&start).r[3];
		_vector Look = startPos - XMLoadFloat3(&m_fLastHandPos);

		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * 30.f * fTimeDelta); //위치 줄이기
		Shoot_Hand(startPos, Player);

		_float4x4 HandMatrix = Player->GetAnimator()->Find_Matrix(m_HandAttached);
		memcpy(&HandMatrix.m[3], &m_fLastHandPos, sizeof _float3);
		Player->GetAnimator()->Change_Final_BoneMatices(m_HandAttached, HandMatrix);

		_vector CheckPos{};
		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - startPos));
		if (fDis < 0.3f)
		{
			m_bEndInHand = true;
		}
	}
	if (!m_bReFinished && m_bEndInHand)
	{
		m_bReFinished = true;
		Player->Set_ActionState(false);
		if (m_bLeftHand)
			Player->Change_Animation(PLAYER_ANIME::SHOOTIN_L, false);
		else if (m_bRightHand)
			Player->Change_Animation(PLAYER_ANIME::SHOOTIN_R, false);
		else if (m_bTwoHand)
			Player->Change_Animation(PLAYER_ANIME::SHOOT_IN, false);
		Player->Set_ActionState(true);

	}

	if (m_bReFinished && Player->Get_Finished())
	{
		Player->Set_ActionState(false);
		Player->Get_AnimeState().bHand = false;
		Player->Change_Animation(PLAYER_ANIME::IDLE, true);

		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine)) return;

		pMachine->Change_State(FSM::END);

	}

	switch (eMove)
	{
	case MOVE::RIGHT:
		Player->Get_Transform()->Go_Right(fTimeDelta);
		break;
	case MOVE::FORWARD:
		Player->Get_Transform()->Go_Straight(fTimeDelta);
		break;
	case MOVE::LEFT:
		Player->Get_Transform()->Go_Left(fTimeDelta);
		break;
	case MOVE::BACKWARD:
		Player->Get_Transform()->Go_BackWard(fTimeDelta);
		break;
	}
}

void CFSM_Hand::Exit_State()
{
	m_bLeftHand = false;
	m_bRightHand = false;
	m_bTwoHand = false;
}

void CFSM_Hand::Shoot_Hand(_fvector startPos,  const shared_ptr<CPlayer> pPlayer)
{

	_float3 LastPos{}, NextPos{}, fLook{};

	for (int32_t i = 0; i < m_ShootBone[m_iArrow].size(); ++i)
	{
		_float t = (_float)i / ((_float)m_ShootBone[m_iArrow].size() - 1.f);
		_float3 LerpPos{}, fRight{}, fUp = { 0,1,0 };

		XMStoreFloat3(&LerpPos, XMVectorLerp(startPos, XMLoadFloat3(&m_fLastHandPos), t));
		_float4x4 matrix = pPlayer->GetAnimator()->Find_Matrix(m_ShootBone[m_iArrow][i]);;
		if (i + 1 < m_ShootBone[m_iArrow].size())
		{
			_float NextT = (_float)(i + 1) / ((_float)m_ShootBone[m_iArrow].size() - 1.f);
			XMStoreFloat3(&NextPos, XMVectorLerp(startPos, XMLoadFloat3(&m_fLastHandPos), NextT));
			XMStoreFloat3(&fLook, XMVector3Normalize(XMLoadFloat3(&NextPos) - XMLoadFloat3(&LerpPos)));
			XMStoreFloat3(&fRight, XMVector3Cross(XMLoadFloat3(&fUp), XMLoadFloat3(&fLook)));
			XMStoreFloat3(&fUp, XMVector3Cross(XMLoadFloat3(&fLook), XMLoadFloat3(&fRight)));

			_float3 Scale[3]{};
			memcpy(&Scale[0], matrix.m[0], sizeof _float3);
			memcpy(&Scale[1], matrix.m[1], sizeof _float3);
			memcpy(&Scale[2], matrix.m[2], sizeof _float3);
			for (int32_t i = 0; i < 3; ++i)
				XMStoreFloat3(&Scale[i], XMVector3Length(XMLoadFloat3(&Scale[i])));

			XMStoreFloat3(&fRight, Scale[0].x * XMLoadFloat3(&fRight));
			XMStoreFloat3(&fUp, Scale[1].x * XMLoadFloat3(&fUp));
			XMStoreFloat3(&fLook, Scale[2].x * XMLoadFloat3(&fLook));

			memcpy(matrix.m[0], &fRight, sizeof _float3);
			memcpy(matrix.m[1], &fUp, sizeof _float3);
			memcpy(matrix.m[2], &fLook, sizeof _float3);

		}
		memcpy(matrix.m[3], &LerpPos, sizeof _float3);
		pPlayer->GetAnimator()->Change_Final_BoneMatices(m_ShootBone[m_iArrow][i], matrix);
	}
}

void CFSM_Hand::Mouse_Cal()
{
	POINT pt{};
	GetCursorPos(&pt);
	_float4x4 CameProj = *CGameInstance::Get().Get_Transform_Inverse(D3DTS::PROJ);
	_float4x4 CamView  = *CGameInstance::Get().Get_Transform_Inverse(D3DTS::VIEW);
	_float2   ViewSize = CGameInstance::Get().Get_ViewportSize();


	_float rayX = (2.f  * pt.x / ViewSize.x- 1.f) / CameProj(0, 0);
	_float rayY = (-2.f * pt.y / ViewSize.y+ 1.f) /  CameProj(1, 1);

	//뷰포트에서의 광선 정의9
	_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector rayDir = XMVectorSet(rayX, rayY, 1.f, 0.f);

	//월드 좌표로 변환
	_matrix InverseView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&CamView));

	XMStoreFloat3(&m_fMouseLook,XMVector3Normalize(XMVector3TransformNormal(rayDir, InverseView)));

}


unique_ptr<CFSM_Hand>		CFSM_Hand::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto		pInstance = unique_ptr<CFSM_Hand>(new CFSM_Hand(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed CShader");
		return nullptr;
	}

	return pInstance;

}
shared_ptr<CPrototype> CFSM_Hand::Clone(void* pArg)
{
	auto		pInstance = shared_ptr<CFSM_Hand>(new CFSM_Hand(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed CShader Clone");
		return nullptr;
	}

	return pInstance;

}