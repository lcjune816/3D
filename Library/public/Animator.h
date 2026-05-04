#pragma once

#include "Component.h"
NS_BEGIN(Engine)
class CAnimation;

class ENGINE_DLL CAnimator : public CComponent
{
public:
	typedef struct strAnimatiordesc 
	{
		shared_ptr<CAnimation>		pCurretAnimation;
		uint32_t					iBoneCnt;
		_float4x4					PreTransform;
	}ANIMATOR_DESC;

private:
	CAnimator(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAnimator(const CAnimator& Prototype);
public:
	virtual ~CAnimator();

private:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* pArg);
public:
	void						Update(_float fTimeDelta);
	
	void						Update_Socket(const _matrix& fMat);

	void						Player_Animation(unique_ptr<CAnimation> pAin);						 
	void						CalculateBoneAnimation(const AssimpNodeData* node, FXMMATRIX parentsTrans);
	void						CalculateFinalBoneMatrices();
	void						CalculateSocketBoneMatrix(const AssimpNodeData* node, FXMMATRIX parentsTrans);
	
	vector<_float4x4>			Get_FinalBoneMatrix() { return m_FinalBoneMatrices; }
	vector<string>&				Get_NameList();
	const uint32_t				Get_BoneCnt() { return m_iBoneCnt; }
	void						Change_Animation(const string& name);
	void						Change_Animation_Enum(uint32_t iNumber, _bool bLoop = true);
	void						Change_Final_BoneMatices(const string& str,_float4x4 boneMatrix);


	const _float4x4				Find_Matrix(const string name) {
		auto find = m_beforeOffsetMatrix.find(name);
		if (find != m_beforeOffsetMatrix.end())
			return find->second;

		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixIdentity());
		return matrix;
		
	}
	_bool						Animation_End();
private:
	
	shared_ptr<CAnimation>		m_pCurrentAnimation		= { nullptr };
	
	vector<uint32_t>			m_IndexNumber		= {};
	vector<_float4x4>			m_GlobalBoneMatrices = {};
	vector<_float4x4>			m_FinalBoneMatrices		= {};
	
	map<string, uint32_t>		m_GlobalBoneMap;
	map<string, _float4x4>		m_beforeOffsetMatrix = {};

	_float						m_fCurrentTime			= { 0 };
	_float						m_fNoLoopTime			= { 0 };
	_float						m_fDeltaTime			= { 0 };
	uint32_t					m_iAnimationNumber		= { 0 };
	uint32_t					m_iBoneCnt				= { 0 };
	_bool						m_bFinished = { false };
	_bool						m_bLoop = { true };
	
	_float4x4					m_PreTransform;
	_float4x4					m_matScale;
	_float4x4					m_RootNode;
public:
	static unique_ptr<CAnimator> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};

NS_END

