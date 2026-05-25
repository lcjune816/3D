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
		map<string, int32_t>        m_BoneList;
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
	

	void						Player_Animation(unique_ptr<CAnimation> pAin);						 
	void						CalculateBoneAnimation(const AssimpNodeData* node, FXMMATRIX parentsTrans);
	void						BlendingBoneAnimation(const AssimpNodeData* Currentnode,const AssimpNodeData* PreNode, FXMMATRIX PreMatrix, const _float& fBlendTime);
	void						CalculateFinalBoneMatrices();
	const AssimpNodeData* Find_Node_By_Name(const AssimpNodeData* pNode, const uint32_t& index);
	void						Stop_Animation(_bool bStop) { m_bStop = bStop; }
	void						Bind_Resource_BoneMatrix(class CShader* pShader, const _char* constName);
	vector<_float4x4>			Get_FinalBoneMatrix() { return m_FinalBoneMatrices; }
	vector<string>&				Get_NameList();
	const uint32_t				Get_BoneCnt() { return m_iBoneCnt; }
	void						Change_Animation(const string& name);
	_bool						Change_Animation_Enum(uint32_t iNumber, _bool bLoop = true, _bool bForce = false,_bool Blend = true);
	void						Change_Final_BoneMatices(const  uint32_t str,_float4x4 boneMatrix);
	_matrix						Find_ChangeBone(uint32_t index);
	int32_t						Find_Key(const string& str)
	{
		auto find = m_BoneList.find(str);
			if(find != m_BoneList.end())
				return find->second;
		
			return -1;
	}

	const _float4x4				Find_Matrix(uint32_t index) {
		auto find = m_beforeOffsetMatrix.find(index);
		if (find != m_beforeOffsetMatrix.end())
			return find->second;

		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixIdentity());
		return matrix;
		
	}
	_bool						Animation_End();
private:
	
	shared_ptr<CAnimation>		m_pCurrentAnimation		= { nullptr };
	shared_ptr<CAnimation>		m_pPreAnimation = { nullptr };

	vector<uint32_t>			m_IndexNumber		= {};
	vector<_float4x4>			m_GlobalBoneMatrices = {};
	vector<_float4x4>			m_FinalBoneMatrices		= {};
	map<string, int32_t>       m_BoneList = {};

	map<uint32_t, uint32_t>			m_GlobalBoneMap;
	map<uint32_t, _float4x4>		m_beforeOffsetMatrix = {};

	_float						m_fCurrentTime			= { 0 };
	_float						m_fNoLoopTime			= { 0 };
	_float						m_fDeltaTime			= { 0 };
	
	_float						m_fPreCurrentTime = { 0 };
	_float						m_fPreNoLoopTime = { 0 };
	_float						m_fPreDeltaTime = { 0 };

	_float						m_fBlendDuration{ 0.3 };
	_float						m_fBlendElapsed{ 0 };

	_float						m_fStopTick = { 0 };
	_float						m_fStopTime = { 0.f };
	uint32_t					m_iAnimationNumber		= { 0 };
	uint32_t					m_iBoneCnt				= { 0 };


	_bool						m_bFinished = { false };
	_bool						m_bLoop = { true };
	_bool						m_bStop = { false };
	_bool						m_bForce = {false};
	_float4x4					m_PreTransform;
	_float4x4					m_matScale;
	_float4x4					m_RootNode;

	_bool						m_bIsBlending = { false };
	int32_t					m_iPreAnimation = {-1};
public:
	static unique_ptr<CAnimator> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};

NS_END

