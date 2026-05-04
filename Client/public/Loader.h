#pragma once
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CLoader
{
private:
	CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CLoader();

public:
	HRESULT Initialize(LEVEL eNextLevelIndex);
	HRESULT	Loading();
	_bool isFinished() const { return m_isFinished; }

private:
	LEVEL					m_eNextLevelIndex = { LEVEL::END };
	ComPtr<ID3D11Device>	m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

	HRESULT					Load_Data(LEVEL eLevel, const _wstring strFilePath, const _wstring strLayerName, const _wstring strObjName, const string strLoadDataName);
private:
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_CriticalSection = {};
private:
	_bool					m_isFinished = { false };

	list<shared_ptr<Engine::CGameObject>>		m_Objects;
private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_GasProduction();

public:
	static unique_ptr<CLoader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>, LEVEL eNextLevelIndex);
};

NS_END
