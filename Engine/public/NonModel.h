#pragma once
#include "MeshModel.h"


NS_BEGIN(Engine)
class CMesh;
class ENGINE_DLL CNonModel : public CMeshModel
{
public:
	typedef struct  strnonModeldesc
	{
		vector<VERTEX_NOANIME> Vertex;
		vector<uint32_t>		Indices;
		vector<TEXTURE>			Textures;
	}NONMODEL_DESC;

protected:
	CNonModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CNonModel(const CNonModel& Prototype);
public:
	virtual  ~CNonModel();

	virtual HRESULT							Initialize(void* pArg);
	virtual HRESULT							Initialize_Prototype();
	virtual HRESULT							Update(_float fTimeDelta);
	virtual HRESULT							Render();

public:
	static unique_ptr<CNonModel>Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);

	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END