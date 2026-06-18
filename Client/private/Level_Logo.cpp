#include "Level_Logo.h"
#include "GameInstance.h"

#include "Level_Loading.h"
CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CLevel {pDevice, pContext}
{

}

CLevel_Logo::~CLevel_Logo()
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(L"Layer_GameObject")))
		return E_FAIL;

	if (FAILED(Ready_Layer_WorldObject(L"Layer_WorldObject")))
		return E_FAIL;



	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{

		if (FAILED(CGameInstance::Get().Change_Level(ETOUI(LEVEL::LOADING),
			CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	
	
}	

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_WorldObject(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Component(const _wstring& strLayerTag)
{
	
	return E_NOTIMPL;
}

unique_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<CLevel_Logo>(new CLevel_Logo(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed Create CLevel_Logo");

	return pInstance;
}
