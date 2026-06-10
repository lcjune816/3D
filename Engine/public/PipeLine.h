#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
class CPipeLine
{
private:
	CPipeLine();
public:
	~CPipeLine();

public:
	const _float4x4* Get_Transform(D3DTS eState);
	const _float4x4* Get_Transform_Inverse(D3DTS eState);
	const _float4* Get_CamPosition();

public:
	void	Set_Transform(D3DTS eState, _fmatrix TransformMatrix);
	HRESULT	 MultiByteWstringToChar(const _wstring& strConvertwstring,  string& strDest);
	HRESULT   MultiByteCharToWstring(const string& strConvertstring,   _wstring& strDest);

public:
	HRESULT Initialize();
	void Update();

private:
	_float4x4		m_TransformStateMatrices[ETOUI(D3DTS::END)] = {};
	
	_float4x4		m_TransformStateInverseMatrices[ETOUI(D3DTS::END)] = {};

	_float4			m_vCamPosition = {};

public:
	static	unique_ptr<CPipeLine> Create();
};

NS_END

