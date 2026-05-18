#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
public:
	typedef struct strBufferdesc
	{
		_float3 fPos[ETOUI(EPOINT::END)];
	}BUFFER_DESC;
protected:
	CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CVIBuffer();

public:
	virtual HRESULT	 Initialize_Prototype();
	virtual HRESULT  Initialize(void* pArg);
	virtual HRESULT	Bind_Resource();
	HRESULT Bind_Resource_InstanceCount(const uint32_t iNumber);
	HRESULT	Bind_ResourceSRV(class CShader* pShader, const _char* pName, aiTextureType eType, uint32_t iTextureNumber );
	virtual HRESULT Render();
	HRESULT Render_Array(uint32_t iArrayIndex);
protected:
	ComPtr<ID3D11Buffer>			m_pVB = { nullptr };
	ComPtr<ID3D11Buffer>			m_pIB = { nullptr };

protected:
	int32_t								   m_iNumVertexBuffers = {};
	int32_t								   m_iVertexStride = {};
	int32_t								   m_iNumVertices = {};
	int32_t								   m_iIndexStride = {};
	int32_t								   m_iNumIndices = {};
											   
	DXGI_FORMAT								   m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY				   m_ePrimitiveType = {};

	vector<TEXTURE>								m_TextureList[AI_TEXTURE_TYPE_MAX];
public:
	virtual shared_ptr<CPrototype> Clone(void* pArg) = 0;
};

NS_END

