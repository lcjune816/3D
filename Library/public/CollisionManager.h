#pragma once

#include "Prototype.h"

NS_BEGIN(Engine)
class CCollisionManager final
{
	typedef struct Edge
	{
		_vector start;
		_vector end;
	}_Edge;
private:
	CCollisionManager();
public:
	virtual ~CCollisionManager();

public:
	weak_ptr<class CGameObject> Check_Ray(int32_t iLayerLevelIndex, const _wstring& strLayerTag , COLLISION_DESC& tagCollision);

	_bool					OBB_Collision(FXMMATRIX matSrc, _fvector vRayPos, _fvector vRayDir, _float fDistance);
	_bool					ABB_Collision(CGameObject* pObj, _fvector vRayPos, _fvector vRayDir,_float& distance);
	_bool					Get_Collision() { return m_bCollision; }
	
	_bool					Only_AABB_Collision(const weak_ptr<class CTransform> pSrcTransform, const weak_ptr<class CTransform>pDstTransform, _bool bBack = true, COLLISION_INFO* pstrCollision = nullptr);
	_bool					Only_AABB_Collision(CTransform* pSrcTransform,  _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix, vector<GRAB_ARM_EDGE>& EdgePoses);
	
	_bool					RayCast(const uint32_t endLayerIndex, const _wstring& strCompareLayerName, const _wstring& LayerName , const _char* tagName, weak_ptr<class CTransform> pSrcTransform, _fvector OffsetRay = {0,0,0,1});
	void					RayCalculator(class CTransform* pSrcTrasnform, class CTransform* pDstTransform, _vector& SrcPos,  _vector& vCenter, _vector& Extern, _hvector offset);
	CGameObject*			AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName,weak_ptr<CGameObject> pObj, _bool bBack = true);
	weak_ptr<CGameObject>	AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix, vector<GRAB_ARM_EDGE>& EdgePoses,_bool bFinished = false);
	
	void					Add_Check_Collision(COLLISION eCollisionValue, weak_ptr<CGameObject> pObj)
	{
		m_CollisionCheckList[eCollisionValue].push_back(pObj); 
	}
	weak_ptr<CGameObject>	Matrix_Check_Collision(_fmatrix Checck,COLLISION eCollisionValue);
	void					Set_Collision(_bool bCollision) { m_bCollision = bCollision; }
	
private:
	_float          m_fMax{FLT_MAX};
	_bool			m_bCollision;
	map<COLLISION,vector<weak_ptr<CGameObject>>>	m_CollisionCheckList;
	
public:
	static unique_ptr<CCollisionManager> Create();
};

NS_END
