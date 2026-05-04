#pragma once
#include "MeshModel.h"


NS_BEGIN(Engine)
class CMesh;
class ENGINE_DLL C3DModel : public CMeshModel
{
public:
	typedef struct  str3DModeldesc
	{

	}MODEL_DESC;

protected:
	C3DModel(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext);
	C3DModel(const C3DModel& Prototype);
public:
	virtual  ~C3DModel();

	virtual HRESULT							Initialize(void* pArg);
	virtual HRESULT							Initialize_Prototype();
	HRESULT									Update(_float fTimeDelta);
	HRESULT									Render();

	void									Add_Animator(shared_ptr<class CAnimator> pAnimator,uint32_t	iBoneCnt);
	const vector<string>&					Get_NameList();
	
	vector<_float4x4>						Get_FinalBoneMatrix();
	uint32_t								Get_BoneCount(){return m_iBoneCount;}
	void									Change_Animation(const string& name);
	void									Change_Animation_Enum(uint32_t iAnimationNumber, _bool bLoop);
	_bool									Animation_End();
private:

	shared_ptr<class CAnimator>				m_pAnimator = { nullptr };
	vector<string>							m_MeshNameList = {};

	uint32_t								m_iBoneCount = {0};	
public:
	static unique_ptr<C3DModel>Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);

	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END