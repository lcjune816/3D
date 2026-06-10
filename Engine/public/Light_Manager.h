#pragma once
#include "Engine_Defines.h"

//vector g_vLightDIr = vector(1.f, -1.f, 1.f, 0.f);
////오른쪽 + 위쪽에서아 애라로 + 앞쪽 방향으로
//
//vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
////빛의 기본 색상 지금은 흰색임
//
//vector g_vLightAmbient = vector(0.6f, 0.6f, 0.6f, 1.f);
////환경광 색 방향없이 전체적으로 깔리는 밝기
//
//vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
////빛의 하이라이트 색 반짝이는 부분
//
////오브젝트 머테리얼 뭐드라 난반사랑 정반사
//vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
//vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
////재질의 반짝임 세기 크면 반짝임이 강하게 보임

NS_BEGIN(Engine)
class CLight_Manager final
{
private:
	CLight_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~CLight_Manager();

public:
	HRESULT Initialize();

	LIGHT_DESC*			  Find_LightMtrl(const LIGHT& eLight);
	void				 Set_LightDesc(LIGHT_DESC& eLight);
	const vector<string>& Get_ObejctNames() { return m_ObjectNames; }

	LIGHT_HANDLE		 Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT				 Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer);
	
	void				Save_Lights(const _wstring& path, const string& strJsonKeyName);
	HRESULT				Load_Lights(uint32_t iPrototypeLevel, const wstring& strPrototypeName, uint32_t iLevel ,const _wstring& strLevelName, json& j);

	void				Light_Dead();
#ifdef _DEBUG
	HRESULT				 Render_Debug_Lights();
#endif 
shared_ptr<class CLight>	 Select_Light(_fvector OriginPos, _fvector OriginDir);

LIGHT_DESC* Get_LightToHandle(uint32_t iIndex, uint32_t iHandle);
private:
	HRESULT		Load_LightMtrl();

private:
	ComPtr<ID3D11Device>			m_pDevice; 
	ComPtr<ID3D11DeviceContext>		m_pContext;

	vector<uint32_t>				m_FreeList;
	vector<uint32_t>				m_Generation;


	vector<shared_ptr<class CLight>>		m_Lights[ETOUI(USETYPE::END)];

	map<string, shared_ptr<LIGHT_VALUE>>			m_LightMtrls;
	vector<string>									m_ObjectNames;

	shared_ptr<class CShader>							m_pDebugShader{ nullptr };
	shared_ptr<class CCube>								m_pDebugVIBuffer{ nullptr };
public:
	static unique_ptr<CLight_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END

