#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class  CMeshNonAnime : public  CVIBuffer
{
public:
	typedef struct tagNonAnime
	{
		vector<VERTEX_NOANIME>  Vertexes;
		vector<uint32_t>		Indices;
		vector<TEXTURE>			Textures[AI_TEXTURE_TYPE_MAX];
		INSTANCING_DESC*		InstanceData{ nullptr };
		MATERIAL				eMatrial;
	}NONANIME_DESC;

private:
	CMeshNonAnime(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	virtual ~CMeshNonAnime();

	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	const MATERIAL						Get_Matrial() { return m_eMatrial; }
private:
	HRESULT							Setup_Mesh(vector<VERTEX_NOANIME>& vertices, vector<uint32_t>& indeces);
private:

	MATERIAL						m_eMatrial;

public:
	static  unique_ptr<CMeshNonAnime> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContextconst);
	virtual shared_ptr<CPrototype>Clone(void* pArg) override;
};


NS_END