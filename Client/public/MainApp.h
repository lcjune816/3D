#pragma once

#include "Client_Defines.h"

namespace Client
{
	class CMainApp
	{
	private:
		CMainApp();
	public:
		~CMainApp();

	public:
		HRESULT	Initialize();
		void	Update(float fTimeDelta);
		HRESULT	Render();

		HRESULT  Ready_Prototype();
		

	private:
		ComPtr<ID3D11Device>				m_pDevice;
		ComPtr<ID3D11DeviceContext>			m_pContext;
	
	private:
		HRESULT Start_Level(LEVEL eStartLevel);
	
	public:
		static unique_ptr<CMainApp> Create();


	};
}

