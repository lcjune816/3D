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

	HRESULT				  Add_LightMtrl(const string tagLight);
	weak_ptr<LIGHT_VALUE> Find_LightMtrl(const string tagLightName);
	const vector<string>& Get_ObejctNames() { return m_ObjectNames; }

	HRESULT				 Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT				 Render(shared_ptr<class CShader> pShader, shared_ptr<class CRect> pVIBuffer);
private:
	HRESULT		Load_LightMtrl();

private:
	ComPtr<ID3D11Device>			m_pDevice; 
	ComPtr<ID3D11DeviceContext>		m_pContext;

	list<shared_ptr<class CLight>>		m_Lights;

	map<string, shared_ptr<LIGHT_VALUE>>			m_LightMtrls;
	vector<string>									m_ObjectNames;
public:
	static unique_ptr<CLight_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

};

NS_END

