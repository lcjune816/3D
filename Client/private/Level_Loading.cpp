#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_GasProduction.h"

#include "UILoadingScreen.h"

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevel_Loading::~CLevel_Loading()
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelIndex)
{
	m_eNextLevelIndex = eNextLevelIndex;

	/* 로딩에 필요한 객체를 생성한다.  */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	/* 다음레벨에 필요한 자원을 로드해주기위한 준비작업(로딩작업을 수행해줄 스레드를 생성한다)을 수행한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelIndex);
	if (nullptr == m_pLoader)
		return E_FAIL;

	CUILoadingScreen::LOADING_SCREEN_DESC LoadingScreenDesc{};
	LoadingScreenDesc.eType = SCREEN::LOADING;

	m_pLoadingScreen = static_pointer_cast<CUILoadingScreen>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_UILoadingScreen", &LoadingScreenDesc));
	if (NULL_TRUE(m_pLoadingScreen)) return E_FAIL;

	LoadingScreenDesc.eType = SCREEN::BLACK;
	m_pBlackBackGround = static_pointer_cast<CUILoadingScreen>(CGameInstance::Get().Clone_Prototype(ETOUI(LEVEL::STATIC), L"Prototype_UILoadingScreen", &LoadingScreenDesc));
	if (NULL_TRUE(m_pBlackBackGround)) return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	m_pLoadingScreen->Update(fTimeDelta);
	if (true == m_pLoader->isFinished() && !m_bLoading)
	{
		static_pointer_cast<CUILoadingScreen>(m_pLoadingScreen)->Change_Screen(SCREEN::END);
		m_bLoading = true;
	}

	if (m_pLoadingScreen->Get_Dead())
	{

		unique_ptr<CLevel>		pNewLevel = { nullptr };

		switch (m_eNextLevelIndex)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::GASZONE:
			pNewLevel = CLevel_GasProduction::Create(m_pDevice, m_pContext);
			break;
		}

		if (FAILED(CGameInstance::Get().Change_Level(ETOUI(m_eNextLevelIndex), std::move(pNewLevel))))
			return;


	}
}

HRESULT CLevel_Loading::Render()
{
	m_pBlackBackGround->Render();
	m_pLoadingScreen->Render();
	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	return S_OK;
}

unique_ptr<CLevel_Loading> CLevel_Loading::Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = unique_ptr<CLevel_Loading>(new CLevel_Loading(pDevice, pContext));

	if (FAILED(pInstance->Initialize(eNextLevelIndex)))
		MSG_BOX("Failed to Created : CLevel_Loading");

	return pInstance;
}
