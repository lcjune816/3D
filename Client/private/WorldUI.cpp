#include "WorldUI.h"
#include "GameInstance.h"
CWorldUI::CWorldUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIObject(pDevice, pContext)
{
}

CWorldUI::CWorldUI(const CUIObject& Prototype) : CUIObject(Prototype)
{
}

CWorldUI::~CWorldUI()
{
}

HRESULT CWorldUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldUI::Initialize(void* pArg)
{
	auto pDesc = static_cast<WORLDUI*>(pArg);

	m_eType = pDesc->eType;

	m_pVIBufferCom = static_pointer_cast<CRect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Rect", nullptr));

	if (NULL_TRUE(m_pVIBufferCom)) return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_UI"), TEXT("Com_Shader"), m_pShaderCom))) return E_FAIL;

	CGameInstance::Get().Add_Decal_Texture("../../UI/Resting_Button.png");
	m_iTextureID[ETOUI(TEXTUREID::DIFFUSE)] = CGameInstance::Get().Find_TextueId("../../UI/Resting_Button.png");
	if (m_iTextureID[ETOUI(TEXTUREID::DIFFUSE)] == -1) return E_FAIL;
	
	CGameInstance::Get().Add_Decal_Texture("../../UI/T_Noise_6.png");
	m_iTextureID[ETOUI(TEXTUREID::NOISE)] = CGameInstance::Get().Find_TextueId("../../UI/T_Noise_6.png");
	if (m_iTextureID[ETOUI(TEXTUREID::NOISE)] == -1) return E_FAIL;

	pDesc->fSizeX= 500.f;
	pDesc->fSizeY = 50.f;
	pDesc->fX = 660.f ;
	pDesc->fY = 100.f ;

	if (pDesc->iLevel == ETOUI(LEVEL::GAMEPLAY))
	{

		switch (m_eType)
		{
		case NOTICE::SPEECH1:
			break;

		case NOTICE::SPEECH2:
			break;

		case NOTICE::SPEECH3:
			break;

		case NOTICE::SPEECH4:
			break;
		}
	}
	else if (pDesc->iLevel == ETOUI(LEVEL::GAMEPLAY))
	{
		switch (m_eType)
		{
		case NOTICE::SPEECH5:
			break;

		case NOTICE::SPEECH6:
			break;
		}
	}
	__super::Initialize(pDesc);

	return S_OK;
}

void CWorldUI::Priority_Update(_float fTimeDelta)
{
	if (CGameInstance::Get().Get_DIKeyState(DIK_F1) & 0x80)
	{
		m_fTimeTick = 0.f;
		m_iFade =6;
	}

	if (CGameInstance::Get().Get_DIKeyState(DIK_1) & 0x80)
		m_eType = NOTICE::SPEECH1;

	if (CGameInstance::Get().Get_DIKeyState(DIK_2) & 0x80)
		m_eType = NOTICE::SPEECH2;

	if (CGameInstance::Get().Get_DIKeyState(DIK_3) & 0x80)
		m_eType = NOTICE::SPEECH3;

	if (CGameInstance::Get().Get_DIKeyState(DIK_4) & 0x80)
		m_eType = NOTICE::SPEECH4;

	if (CGameInstance::Get().Get_DIKeyState(DIK_5) & 0x80)
		m_eType = NOTICE::SPEECH5;

	if (CGameInstance::Get().Get_DIKeyState(DIK_6) & 0x80)
		m_eType = NOTICE::SPEECH6;
	if (CGameInstance::Get().Get_DIKeyOneState(DIMKEYINPUT::A))
		m_bEnd = !m_bEnd;
	__super::Priority_Update(fTimeDelta);
}

void CWorldUI::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	
	if (m_bEnd)
	{

		m_fTimeTick += fTimeDelta;
		if (m_iFade == 6)
		{
			_float fTime = min(m_fTimeTick / 8.f, 1.f);
			m_fAlpha = 0 + (0.8 - 0) * fTime;
			if (fTime >= 1.f)
			{
				m_fTimeTick = 0.f;
				m_iFade = 7;
			}
		}
		else
		{
			_float fTime = min(m_fTimeTick / 2.f, 1.f);
			m_fAlpha = 0.8 + (0 - 0.8) * fTime;
		}
	}
}

void CWorldUI::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::UI, SHARED_THIS(CWorldUI));
}

HRESULT CWorldUI::Render()
{
	if (!m_bEnd)
		return S_OK;
	
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", &m_ViewMatrix);
	m_pShaderCom->Bind_Matrix("g_Projection", &m_ProjMatrix);
	m_pShaderCom->Bind_RawValue("g_Time", &m_fTimeTick,sizeof m_fTimeTick);
	m_pShaderCom->Bind_SRV("g_Diffuse", CGameInstance::Get().Find_Decal_Texture(m_iTextureID[ETOUI(TEXTUREID::DIFFUSE)]));
	m_pShaderCom->Bind_SRV("g_NoiseTexture", CGameInstance::Get().Find_Decal_Texture(m_iTextureID[ETOUI(TEXTUREID::NOISE)]));

	m_pShaderCom->Begin(m_iFade);

	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();
	

	switch (m_eType)
	{
	case NOTICE::SPEECH1:

		CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("전기를 활성화하여 문을 열기"), _float2(500, 85)
			, 1.f,XMVectorSet(m_fAlpha, m_fAlpha, m_fAlpha, 0));
		break;

	case NOTICE::SPEECH2:

		CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("배터리로 전기 활성화 하기"), _float2(510, 85)
			, 1.f, XMVectorSet(m_fAlpha, m_fAlpha, m_fAlpha, 0));
		break;

	case NOTICE::SPEECH3:
		CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("발전기를 작동 시키기"), _float2(545, 85)
			, 1.f, XMVectorSet(m_fAlpha, m_fAlpha, m_fAlpha, 0));
		break;

	case NOTICE::SPEECH4:
		CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("보스를 피해 도망치세요"), _float2(530, 85)
			, 1.f, XMVectorSet(m_fAlpha, m_fAlpha/4.f, m_fAlpha/4.f, 0));
		break;

	case NOTICE::SPEECH5:
		CGameInstance::Get().Draw_Text(TEXT("Font_Default"), TEXT("엘리베이터로 탈출하기"), _float2(530, 85)
			, 1.f, XMVectorSet(m_fAlpha/4.f, m_fAlpha , m_fAlpha / 4.f, 0));
		break;

	case NOTICE::SPEECH6:
		break;
	}
	
	return S_OK;
}


unique_ptr<CWorldUI>	CWorldUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CWorldUI>(new CWorldUI(pDevice, pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CWorldUI::Clone(void* pArg)
{

	auto pInstance = shared_ptr<CWorldUI>(new CWorldUI(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
