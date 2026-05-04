#pragma once
#include "Component.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CMesh;
class ENGINE_DLL CMeshModel : public CComponent
{
public:
	typedef struct  strnonModeldesc
	{
	}PRMODEL_DESC;
private:
	typedef struct str3DModelfMaxfMin
	{
		_float3			fMin;
		_float3			fMax;

	}FMAX_FMIN;

protected:
	CMeshModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMeshModel(const CMeshModel& Prototype);
public:
	virtual  ~CMeshModel();

public:
	 void							Add_Mesh(shared_ptr<CVIBuffer> pMesh);
	 void							Add_MeshName(string Name);
	 void							Set_MAX_MIN(_float3 fMax, _float3 fMin) { m_fMaxMin.fMin = fMin; m_fMaxMin.fMax = fMax; }
	 _float3						Get_Max() { return m_fMaxMin.fMax; }
	 _float3						Get_Min() { return m_fMaxMin.fMin; }

	 void							Set_Shader(shared_ptr<class CShader> pShader);
protected:
	virtual HRESULT					Initialize(void* pArg);
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Update(_float fTimeDelta);
	virtual HRESULT					Render();

protected:

	vector<shared_ptr<CVIBuffer>>			m_pMeshList;
	vector<uint32_t>						m_MeshNameList = {};

	FMAX_FMIN								m_fMaxMin;

	weak_ptr<class CShader>					m_pShader;
public:
	static unique_ptr<CMeshModel>Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);

	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END