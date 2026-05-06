#include "FSM_LeftHand.h"
#include "Player_LeftHand.h"
#include "GameInstance.h"
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
	m_ShootBone.push_back("JNT_L_Grabpack_Tube_01");
	m_ShootBone.push_back("JNT_L_Grabpack_Tube_02");
	m_ShootBone.push_back("JNT_L_Grabpack_Tube_03");
	m_ShootBone.push_back("JNT_L_Grabpack_Tube_04");
	m_ShootBone.push_back("JNT_L_Grabpack_Tube_05");
	return S_OK;
}

void CFSM_LeftHand::Enter_State()
{
	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player)) return;


	m_bLeftHand = Player->Get_AnimeState().bLHand;
	strcpy_s(m_HandName, sizeof(m_HandName), "JNT_L_Grabpack_Tube_06");
	strcpy_s(m_FirstHand, sizeof(m_FirstHand), "JNT_L_Grabpack_Gun");
	strcpy_s(m_HandAttached, sizeof(m_HandAttached), "JNT_L_HandAttachment");
	Player->Change_Animation(PLAYER_ANIME::SHOOTOUT_R, true);
	Player->Get_AnimeState().bLHand = false;


	Player->Set_ActionState(true);
	m_fShootMaxTime = 30.f;
	m_fShootTime = 0.f;
	m_fShootTimeTick = 0.f;
	m_bReFinished = false;
	m_bEndHand = false;
	m_bEndInHand = false;
	m_fSpeed = 30.f;

	XMStoreFloat3(&m_fFirstLook, Player->Get_Transform().lock()->Get_State(STATE::LOOK));
	_float4x4 End = Player->GetAnimator()->Find_Matrix(m_HandName);
	memcpy(&m_fLastHandPos, End.m[3], sizeof _float3);
}

void CFSM_LeftHand::Update_State(_float fTimeDelta)
{

	auto Player = m_pPlayer.lock();

	if (NULL_TRUE(Player)) return;

	MOVE eMove = Player->Get_State();

	PLAYER_HAND HandState = m_pHand->Get_PlayerHand();
	Mouse_Cal();

	if (!m_bEndHand && (CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80 || CGameInstance::Get().Get_DIMouseState(DIMK::RBUTTON) & 0x80))
	{
		m_fShootTimeTick += fTimeDelta;

		if (m_fShootTimeTick > 0.05f)
		{
			++m_fShootTime;
			m_fShootTimeTick = 0.f;
		}

		_float4x4 start  = Player->GetAnimator()->Find_Matrix(m_FirstHand); //처음위치
		_vector startPos = XMLoadFloat4x4(&start).r[3];
		if(!m_pHand->Get_HandState().bHandAttached)
			XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + XMLoadFloat3(&m_fFirstLook) * m_fSpeed * fTimeDelta); //마지막 위치 늘려서 보간하기
		
		Shoot_Hand(startPos, Player);

		_float4x4 HandMatrix = Player->GetAnimator()->Find_Matrix(m_HandAttached);
		memcpy(&HandMatrix.m[3], &m_fLastHandPos, sizeof _float3);
		Player->GetAnimator()->Change_Final_BoneMatices(m_HandAttached, HandMatrix);

		if (m_fShootTime >= m_fShootMaxTime)
			m_bEndHand = true;

		if (m_fSpeed >= 40)
			m_fSpeed = 40.f;
	}
	else m_bEndHand = true;

	if (!m_bReFinished && m_bEndHand && !m_pHand->Get_HandState().bHandAttached)
	{
		_float4x4 start = Player->GetAnimator()->Find_Matrix(m_FirstHand); //처음위치
		_vector startPos = XMLoadFloat4x4(&start).r[3];
		_vector Look = startPos - XMLoadFloat3(&m_fLastHandPos);

		XMStoreFloat3(&m_fLastHandPos, XMLoadFloat3(&m_fLastHandPos) + Look * m_fSpeed * 0.5f * fTimeDelta); //위치 줄이기
		Shoot_Hand(startPos, Player);

		_float4x4 HandMatrix = Player->GetAnimator()->Find_Matrix(m_HandAttached);
		memcpy(&HandMatrix.m[3], &m_fLastHandPos, sizeof _float3);
		Player->GetAnimator()->Change_Final_BoneMatices(m_HandAttached, HandMatrix);

		_vector CheckPos{};
		_float fDis = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&m_fLastHandPos) - startPos));
		if (fDis < 0.9f * 0.9f)
		{
			m_bEndInHand = true;
		}
	}

	Hand_End(Player.get());
}

void CFSM_LeftHand::Exit_State()
{
	m_bLeftHand = false;
	if (m_pHand->Get_HandState().bHandAttached)
		m_pHand->Get_HandState().bHandAttached = false;
}

void CFSM_LeftHand::Set_LeftHand(shared_ptr<CGameObject> pObj)
{
	m_pHand = static_pointer_cast<CPlayer_LeftHand>(pObj);
}

void			CFSM_LeftHand::Hand_End(CPlayer* Player)
{
	if (!m_bReFinished && m_bEndInHand)
	{
		m_bReFinished = true;
		Player->Set_ActionState(false);
		Player->Change_Animation(PLAYER_ANIME::SHOOT_IN, false);
		Player->Set_ActionState(true);

	}
	if (m_bReFinished && (Player->Get_Finished() || CGameInstance::Get().Get_DIMouseState(DIMK::LBUTTON) & 0x80))
	{
		Player->Get_AnimeState().bLHand = false;
		Player->Set_ActionState(false);

		Player->Change_Animation(PLAYER_ANIME::IDLE, true);
		auto pMachine = m_pMachine.lock();
		if (NULL_TRUE(pMachine)) return;

		pMachine->Change_State(FSM::END);

	}
}
void CFSM_LeftHand::Shoot_Hand(_fvector startPos, const shared_ptr<CPlayer> pPlayer)
{

	_float3 LastPos{}, NextPos{}, fLook{};

	for (int32_t i = 0; i < m_ShootBone.size(); ++i)
	{
		_float t = (_float)i / ((_float)m_ShootBone.size() - 1.f);
		_float3 LerpPos{}, fRight{}, fUp = { 0,1,0 };

		XMStoreFloat3(&LerpPos, XMVectorLerp(startPos, XMLoadFloat3(&m_fLastHandPos), t));
		_float4x4 matrix = pPlayer->GetAnimator()->Find_Matrix(m_ShootBone[i]);
	
		memcpy(matrix.m[3], &LerpPos, sizeof _float3);
		pPlayer->GetAnimator()->Change_Final_BoneMatices(m_ShootBone[i], matrix);
	}
}

void CFSM_LeftHand::Mouse_Cal()
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