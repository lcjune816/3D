#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CCollisionManager final
{
private:
	CCollisionManager();
public:
	virtual ~CCollisionManager();

public:
	weak_ptr<class CGameObject> Check_Ray(int32_t iLayerLevelIndex, const _wstring& strLayerTag , COLLISION_DESC& tagCollision);

	_bool			 OBB_Collision(FXMMATRIX matSrc, _fvector vRayPos, _fvector vRayDir, _float fDistance);
	_bool			 ABB_Collision(CGameObject* pObj, _fvector vRayPos, _fvector vRayDir,_float& distance);
	_bool			 Get_Collision() { return m_bCollision; }
	
	_bool			 Only_AABB_Collision(const weak_ptr<class CTransform> pSrcTransform, const weak_ptr<class CTransform>pDstTransform, _bool bBack = true);


	CGameObject*			AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, weak_ptr<CGameObject> pObj, _bool bBack = true);
	void					Set_Collision(_bool bCollision) { m_bCollision = bCollision; }
private:

	_bool			m_bCollision;
public:
	static unique_ptr<CCollisionManager> Create();
};

NS_END
