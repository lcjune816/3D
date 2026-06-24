#include "UILoadingScreen.h"
#include "GameInstance.h"
CUILoadingScreen::CUILoadingScreen(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIObject(pDevice,pContext)
{
}

CUILoadingScreen::CUILoadingScreen(const CUIObject& Prototype) : CUIObject(Prototype)
{
}

CUILoadingScreen::~CUILoadingScreen()
{
}

HRESULT CUILoadingScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUILoadingScreen::Initialize(void* pArg)
{
	auto pDesc = static_cast<LOADING_SCREEN_DESC*>(pArg);

	if (pDesc->iLevel == ETOUI(LEVEL::GASZONE))
	{
		CGameInstance::Get().Add_Observers(WORLD_EVENT::PLAYER_DEAD, SHARED_THIS(CUILoadingScreen));
		CGameInstance::Get().Add_Observers(WORLD_EVENT::END, SHARED_THIS(CUILoadingScreen));
	}
		
	m_eType = pDesc->eType;
	
	m_pVIBufferCom = static_pointer_cast<CRect>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_Rect", nullptr));
	
	if (NULL_TRUE(m_pVIBufferCom)) return E_FAIL;

	if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Component_UI"), TEXT("Com_Shader"), m_pShaderCom))) return E_FAIL;
	const _char* pName =  {};
	UIOBJECT_DESC Desc{};

	if (m_eType == SCREEN::LOADING)
	{
		m_fTexCoord.z = 0.f;
		m_fTexCoord.w = 0.f;
		Desc.fSizeX = 150.f;
		Desc.fSizeY = 150.f;
		Desc.fX = 600.f;
		Desc.fY = 600.f ;
		pName = "../../Orb_32subframes_density.dds";
	}
	else if(m_eType == SCREEN::BLACK)
	{
		Desc.fSizeX = Client::g_iWinSizeX;
		Desc.fSizeY = Client::g_iWinSizeY;
		Desc.fX = Client::g_iWinSizeX / 2.f;
		Desc.fY = Client::g_iWinSizeY / 2.f;
	}
	else
	{

		Desc.fSizeX = Client::g_iWinSizeX;
		Desc.fSizeY = Client::g_iWinSizeY;
		Desc.fX = Client::g_iWinSizeX / 2.f;
		Desc.fY = Client::g_iWinSizeY / 2.f;
		pName = "../../T_Perlin_Noise_M.dds";
	}
	
	if (m_eType != SCREEN::BLACK)
	{
		CGameInstance::Get().Add_Decal_Texture(pName);
		m_iTextureID[ETOUI(TEXTUREID::DIFFUSE)] = CGameInstance::Get().Find_TextueId(pName);

		
		if (m_iTextureID[ETOUI(TEXTUREID::DIFFUSE)] == -1)
			return E_FAIL;
	}
	
	CGameInstance::Get().Add_Decal_Texture("../../UI/T_maskframe_thicker_256.png");
	m_iTextureID[ETOUI(TEXTUREID::DIFFUSE2)] = CGameInstance::Get().Find_TextueId("../../UI/T_maskframe_thicker_256.png");
	
	__super::Initialize(&Desc);

	m_eTextureType = TEXTUREID::DIFFUSE;
	m_fTimeTick = {}, m_fTimeRange = {};
	m_fTexCoord.x = 1.f;
	return S_OK;
}

void CUILoadingScreen::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUILoadingScreen::Update(_float fTimeDelta)
{
	if (m_eType == SCREEN::FADEIN)
		FadeIn(fTimeDelta);
	else if (m_eType == SCREEN::FADEOUT)
		FadeOut(fTimeDelta);
	else if (m_eType == SCREEN::LOADING)
		Loading(fTimeDelta);
	
	__super::Update(fTimeDelta);
}

void CUILoadingScreen::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUILoadingScreen::Render()
{

	if (m_bStop) return S_OK;

	if(m_eType == SCREEN::LOADING)
		m_pShaderCom->Bind_RawValue("g_TexCoord", &m_fTexCoord, sizeof m_fTexCoord);
	else 
		m_pShaderCom->Bind_RawValue("g_Time", &m_fTimeRange, sizeof m_fTimeRange);
	
	if(m_eType != SCREEN::BLACK)
		m_pShaderCom->Bind_SRV("g_Diffuse", CGameInstance::Get().Find_Decal_Texture(m_iTextureID[ETOUI(m_eTextureType)]));
	
	m_pTransform->Bind_Matrix(m_pShaderCom, "g_World");
	m_pShaderCom->Bind_Matrix("g_View", &m_ViewMatrix);
	m_pShaderCom->Bind_Matrix("g_Projection", &m_ProjMatrix);
	m_pShaderCom->Begin(ETOUI(m_eType));

	m_pVIBufferCom->Bind_Resource();
	m_pVIBufferCom->Render();
	return S_OK;
}

void CUILoadingScreen::OnNotify(const EVENT& eEvent)
{
	m_bStop = false;
	if (eEvent.eEvent == WORLD_EVENT::END)
	{
		m_eTextureType = TEXTUREID::DIFFUSE;
		m_eType = SCREEN::BLACK;
	}
	else if (eEvent.eEvent == WORLD_EVENT::PLAYER_DEAD)
	{
		m_eTextureType = TEXTUREID::DIFFUSE2;
		m_eType = SCREEN::DEADSCREEN;
	}
}

void CUILoadingScreen::FadeIn(const _float& fTimeDelta)
{

	m_fTimeTick += fTimeDelta;
	m_fTimeRange = min(m_fTimeTick / 4.f, 1.f);

	m_bDead = m_fTimeRange >= 1.f;

}
void CUILoadingScreen::FadeOut(const _float& fTimeDelta)
{
	if (m_bStop) return;
	
		
	m_fTimeTick += fTimeDelta;
	m_fTimeRange = min(m_fTimeTick / 4.f,1.f);

	m_bDead = m_fTimeRange >= 1.f;
	if (m_bDead)
	{
		m_fTimeTick = 0.f;
		m_bStop = true;
	}
}
void CUILoadingScreen::Loading(const _float& fTimeDelta)
{
	if (m_bEnd)
	{
		m_fLoadingTick += fTimeDelta;

		_float fTime = m_fLoadingTick / 3.f;

		m_fTexCoord.x = 1.f - (0.f + 1.f) * fTime;

		m_bDead = fTime >= 1.f;
	}
	m_fTimeTick += fTimeDelta;
	if (m_fTimeTick> 0.1f)
	{
		m_fTimeTick = 0;
		m_fTexCoord.z += 1.f / 8.f;

		if (m_fTexCoord.z > 1.f - 0.0001f)
		{
			m_fTexCoord.z = 0.f;
			m_fTexCoord.w += 1.f / 4.f;
		}
		if (m_fTexCoord.w > 1.f - 0.0001f)
		{
			m_fTexCoord.w = 0.f;
		}
	}
}

unique_ptr<CUILoadingScreen>	CUILoadingScreen::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CUILoadingScreen>(new CUILoadingScreen(pDevice,pContext));
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
shared_ptr<CPrototype> CUILoadingScreen::Clone(void* pArg)
{

	auto pInstance = shared_ptr<CUILoadingScreen>(new CUILoadingScreen(*this));
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed UILoadingScreen");
		return nullptr;
	}
	return pInstance;
}
