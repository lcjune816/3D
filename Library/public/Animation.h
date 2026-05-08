#pragma once

#include "Component.h"
#include "Bone.h"


NS_BEGIN(Engine)

class CAnimation : public CComponent
{
public:
	typedef struct strAnimationdesc
	{
		vector<ANITIME>								AniTime;
		vector<string>								AnimantionName;
		vector<Bone>								BoneInfo;

		vector<vector<shared_ptr<class CBone>>>		Bones;

		AssimpNodeData								RootNode;

	}ANIMATION_DESC;
	

private:
	CAnimation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CAnimation(const CAnimation& Prototype);
public:
	virtual ~CAnimation();

private:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* pArg);

	HRESULT	Initialize(const string& animationPath, map<string, Bone>& mesh, uint32_t& boneCount);

public:
	CBone*										Find_Bone(const uint32_t index,uint32_t i);
	_float										Get_Duration(uint32_t i) 
	{ 
		if (i == m_AniTime.size())
			return 0.f;

		return m_AniTime[i].m_Duration; }
	_float										Get_Tick(uint32_t i) { 
		if (i == m_AniTime.size())
			return 0.f;

		return m_AniTime[i].m_iTicksPerSecond; }
	AssimpNodeData&								Get_RootNode() { return m_RootNode; }
	vector<Bone>&								Get_BoneInfo() { return m_BoneInfo; }
	vector<string>&								Get_NameList() { return m_AnimantionName; }



private:


	vector<ANITIME>								m_AniTime;
	vector<string>								m_AnimantionName;
	vector<Bone>								m_BoneInfo;


	vector<vector<shared_ptr<class CBone>>>		m_Bones;


	AssimpNodeData								m_RootNode;

public:
	static unique_ptr<CAnimation> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};

NS_END

