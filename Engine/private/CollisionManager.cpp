#include "CollisionManager.h"
#include "GameInstance.h"
#include "Layer.h"
CCollisionManager::CCollisionManager()
{

}

CCollisionManager::~CCollisionManager()
{

}

weak_ptr<CGameObject>  CCollisionManager::Check_Ray(int32_t iLayerLevelIndex, const _wstring& strLayerTag, COLLISION_DESC& tagCollision)
{
	CLayer* pLayer = CGameInstance::Get().Find_Layer(iLayerLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return {};
	_matrix CameProj = XMLoadFloat4x4(&tagCollision.matProj);
	
	_float4x4 Proj = {};
	 
	XMStoreFloat4x4(&Proj, CameProj);

	_float2 ViewPort = CGameInstance::Get().Get_ViewportSize();
	_float rayX = (2.f *  tagCollision.fMouse.x / ViewPort.x - 1.f) / Proj(0, 0);
	_float rayY = (-2.f * tagCollision.fMouse.y / ViewPort.y+ 1.f) / Proj(1, 1);

	//뷰포트에서의 광선 정의9
	_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector rayDir = XMVectorSet(rayX, rayY, 1.f, 0.f);

	//월드 좌표로 변환
	_matrix CamView = XMLoadFloat4x4(&tagCollision.matView);
	_matrix InverseView = XMMatrixInverse(nullptr, CamView);

	rayOrigin = XMVector3TransformCoord(rayOrigin, InverseView);
	rayDir = XMVector3Normalize(XMVector3TransformNormal(rayDir, InverseView));

	_float tDis = 0;
	_float iSrcDistance = FLT_MAX;
	XMStoreFloat3(&tagCollision.fPos, rayOrigin);
	XMStoreFloat3(&tagCollision.fDir, rayDir);

	weak_ptr<CGameObject> pObj = {};
	_matrix InverseWorld = {};
	_vector Worldray = {}, WorldrayDir = {}, OriginRay = {}, OriginDir = {};

	//tagCollision.fMouse 
	for (auto& iter : pLayer->Get_ObjectList())
	{
		//월드로만 비교
		Worldray    = rayOrigin;
		WorldrayDir = rayDir;
		InverseWorld = XMMatrixInverse(nullptr, iter->Get_Transform().lock()->Get_World());
		//대상의 월드 pos를 가져옴
		
		Worldray = XMVector3TransformCoord(Worldray, InverseWorld);
		WorldrayDir = XMVector3Normalize(XMVector3TransformNormal(WorldrayDir, InverseWorld));

		_float3 fMax = iter->Get_Transform().lock()->Get_Max();
		_float3 fMin = iter->Get_Transform().lock()->Get_Min();
		_vector	vDstPos = ((XMLoadFloat3(&fMax) + XMLoadFloat3(&fMin)) * 0.5f);
		_vector P = {};
		//     C
		//t = (대상위치 - 레이위치) 내적 레이방향 
		//P(t) =  레이위치 + t * 레이방향;
		//D  =  |P - C| 이건 길이지?
		_float fDistance;
		tDis = XMVectorGetX(XMVector3Dot(vDstPos - Worldray, WorldrayDir)); //앞 뒤 판정 내적은 앞 뒤 판정
		
		if (tDis < 0.f)
			continue;

		P = Worldray + (WorldrayDir * tDis);	

		fDistance = XMVectorGetX(XMVector3Length(P - vDstPos));
			

		
		//반지름 판정
		if (fDistance > XMVectorGetX( XMVector4Length(((XMLoadFloat3(&fMax) - XMLoadFloat3(&fMin))) *0.5f)))
			continue;
		_float fDis(0);
		if (ABB_Collision(iter.get(), Worldray, WorldrayDir, fDis))
		{
			if (iSrcDistance > fDis)
			{
				iSrcDistance = fDis;
				pObj = iter;
				
			}
		}
		

	}

	if (pObj.lock() != nullptr && GetKeyState(VK_LBUTTON) & 0x8002)
		return pObj;
		tagCollision.fPos.x = tDis;


	return {};
}

_bool CCollisionManager::OBB_Collision(FXMMATRIX matSrc, _fvector vRayPos, _fvector vRayDir, _float fDistance)
{

	_matrix InverseWorld = matSrc;
	_matrix matRayWorld = XMMatrixIdentity();
	_vector vPos = vRayPos;
	_vector vRayD = vRayDir;
	
	_vector vDestAxis[3] = {};
	_vector vSrcAxis[3] = {};
	
	_vector ScaleX{}, ScaleY{}, ScaleZ{};
	memcpy(&InverseWorld.r[0], &ScaleX, sizeof(_vector));
	memcpy(&InverseWorld.r[1], &ScaleY, sizeof(_vector));
	memcpy(&InverseWorld.r[2], &ScaleZ, sizeof(_vector));

	_float3 Scale = { XMVectorGetX(XMVector4Length(ScaleX)), XMVectorGetX(XMVector4Length(ScaleY)),XMVectorGetX(XMVector4Length(ScaleZ)) };
	
	_vector vDistance = InverseWorld.r[3] - vRayPos;
	memcpy(&matRayWorld.r[3], &vRayPos, sizeof(_vector));

	for (int32_t i = 0; i < 3; ++i)
	{
		memcpy(&vDestAxis[0], &InverseWorld.r[i], sizeof(_vector)); // 오브젝트
		memcpy(&vSrcAxis[0], &matRayWorld.r[i], sizeof(_vector));   // 마우스
		vDestAxis[i] = XMVector4Normalize(vDestAxis[i]);
		vSrcAxis[i]  = XMVector4Normalize(vSrcAxis[i]);
	}
	vDestAxis[0] = Scale.x * vDestAxis[0];
	vDestAxis[1] = Scale.y * vDestAxis[1];
	vDestAxis[2] = Scale.z * vDestAxis[2];

	for (int32_t i = 0; i < 3; ++i)
	{
		_float fDot = fabsf(XMVectorGetX(XMVector4Dot(vDistance, vSrcAxis[i])));//중심 거리 벡터를 현재 플레이어 축에서 투영한 길이
		_float rA(0), rB(0); //현재 중심 - 중심의 길이와 축 하나를 Dot할 경우 해당 축기준으로 바라본 중심의 길이가 나옴
		
		rA = fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[0], vSrcAxis[i]))) + //right up look을 을 각각 비교 축 기준으로 비교하여 더한 길이의 값
			 fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[1], vSrcAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[2], vSrcAxis[i])));

		rB = fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[0], vSrcAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[1], vSrcAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[2], vSrcAxis[i])));

		if ((fDot > fabsf(rA + rB)))
			{
				return true;
			}
	}

	for (int32_t i = 0; i < 3; ++i)
	{
		_float fDot = fabsf(XMVectorGetX(XMVector4Dot(vDistance, vDestAxis[i])));//중심 거리 벡터를 현재 플레이어 축에서 투영한 길이
		_float rA(0), rB(0); //현재 중심 - 중심의 길이와 축 하나를 Dot할 경우 해당 축기준으로 바라본 중심의 길이가 나옴

		rA = fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[0], vDestAxis[i]))) + //right up look을 을 각각 비교 축 기준으로 비교하여 더한 길이의 값
			 fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[1], vDestAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vSrcAxis[2], vDestAxis[i])));

		rB = fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[0], vDestAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[1], vDestAxis[i]))) +
			 fabsf(XMVectorGetX(XMVector4Dot(vDestAxis[2], vDestAxis[i])));

		if ((fDot > fabsf(rA + rB)))
		{
			return true;
		}
	}
	return false;
}

_bool CCollisionManager::ABB_Collision(CGameObject* pObj, _fvector vRayPos, _fvector vRayDir, _float& distance)
{

	_float3 vMax = pObj->Get_Transform().lock()->Get_Max();
	_float3 vMin = pObj->Get_Transform().lock()->Get_Min();
	_float3 vDst = { };
	_float3 fRay = {};
	_float3 fDir= {};



	_vector fCenter = { 0,0,0,0 };
	//     C
	//t = (대상위치 - 레이위치) 내적 레이방향 
	//P(t) =  레이위치 + t * 레이방향;
	//D  =  |P - C| 이건 길이지?
	_vector Dir = XMVector3Normalize(vRayDir);
	XMStoreFloat3(&fRay, vRayPos);
	XMStoreFloat3(&fDir, Dir);


	//대상위치의 좌표 - 내 위치 좌표 / 방향을 나누기
	vMin.x = (vMin.x - fRay.x) / fDir.x; //시간 계산 1 ~ 5초일경우 3 ~ 4초일 경우  4 ~ 5초일경우 4가 겹치니까 현재 거리에서 해당 거리까지 이동시 
	vMax.x = (vMax.x - fRay.x) / fDir.x; //4초때에 3개의 축이 겹치는 부분을 계산
	_float fminX = min(vMin.x, vMax.x);
	_float fmaxX = max(vMin.x, vMax.x);

	vMin.y = (vMin.y - fRay.y) / fDir.y;
	vMax.y = (vMax.y - fRay.y) / fDir.y;
	_float fminY = min(vMin.y, vMax.y);
	_float fmaxY = max(vMin.y, vMax.y);

	vMin.z = (vMin.z - fRay.z)  / fDir.z;
	vMax.z = (vMax.z - fRay.z ) / fDir.z;
	_float fminZ = min(vMin.z, vMax.z);
	_float fmaxZ = max(vMin.z, vMax.z);

	_float fnear = max(max(fminX, fminY), fminZ);
	_float fFar  = min(min(fmaxX, fmaxY), fmaxZ);
	
	if (fnear <= fFar && fFar > 0)
	{
		distance = fnear;
		return true;
	}
	
	return false;
}

_bool CCollisionManager::Only_AABB_Collision(const weak_ptr<CTransform> pSrcTransform, const weak_ptr<CTransform> pDstTransform, _bool bBack, COLLISION_INFO* pstrCollision)
{
	auto   SrcTransform = pSrcTransform.lock();
	auto   DstTransform = pDstTransform.lock();

	_matrix SrcWorld = SrcTransform->Get_World();
	_matrix DstWorld = DstTransform->Get_World();
	_matrix DstInverseWorld = XMMatrixInverse(nullptr, DstWorld);

	_float3 DstMin = DstTransform->Get_Min();
	_float3 DstMax = DstTransform->Get_Max();

	_vector DstLocalCenter = (XMLoadFloat3(&DstMax) + XMLoadFloat3(&DstMin)) * 0.5f;
	_vector DstExtents     = (XMLoadFloat3(&DstMax) - XMLoadFloat3(&DstMin)) * 0.5f;

	_vector SrcLocalPos = XMVector3TransformCoord(SrcWorld.r[3], DstInverseWorld);
	_vector SrcLocalRay = XMVector3Normalize(DstLocalCenter - SrcLocalPos);

	BoundingBox box;
	XMStoreFloat3(&box.Center, DstLocalCenter);
	XMStoreFloat3(&box.Extents, DstExtents);
	
	_float fMaxDist = XMVectorGetX(XMVector3Length(DstLocalCenter - SrcLocalPos ));
	_float fDist{};
	if (box.Intersects(SrcLocalPos, SrcLocalRay, fDist))
	{
		if (fDist < 30.f)
		{
			BoundingBox SrcBox;

			_float3 SrcMin = SrcTransform->Get_Min();
			_float3 SrcMax = SrcTransform->Get_Max();

			_vector SrcExtents = (XMLoadFloat3(&SrcMax) - XMLoadFloat3(&SrcMin)) * 0.5f;

			XMStoreFloat3(&SrcBox.Center,  SrcWorld.r[3]);
			XMStoreFloat3(&SrcBox.Extents, SrcExtents);

			SrcBox.Transform(SrcBox, DstInverseWorld);
			if (SrcBox.Intersects(box))
				return true;
			
			if (NULL_FALSE(pstrCollision))
			{
				XMStoreFloat3(&pstrCollision->DstLocalPos, DstLocalCenter);
				XMStoreFloat3(&pstrCollision->SrcLocalPos, SrcLocalPos);
				XMStoreFloat3(&pstrCollision->LocalExtents, DstExtents);
				pstrCollision->fDist=fDist;

			}
		}
	}
	return false;
}



_bool CCollisionManager::RayCast(const uint32_t endLayerIndex, const _wstring& strCompareLayerName, const _wstring& LayerName, const _char* tagName, weak_ptr<CTransform> pSrcTransform, _fvector OffsetRay)
{
	CLayer* pLayer = nullptr;
	CLayer* ComPareLayer = nullptr;
	for (size_t i = 0; i < endLayerIndex; ++i)
	{
		pLayer = CGameInstance::Get().Find_Layer(i, LayerName);
		ComPareLayer = CGameInstance::Get().Find_Layer(i, strCompareLayerName);
		if (NULL_FALSE(pLayer) && NULL_FALSE(ComPareLayer))
			break;
	}
	if (NULL_TRUE(pLayer) && NULL_TRUE(ComPareLayer))
		return false;
	
	
	auto    pSrcTrans = pSrcTransform.lock();
	if (NULL_TRUE(pSrcTrans))
		return true;
	_vector vPlayerDestPos{}, vPlayerDestLook{};
	_vector vSrcLook = XMVector3Normalize(pSrcTrans->Get_State(STATE::LOOK));
	
	for (auto& pLayerInObj : pLayer->Get_ObjectList())
	{
		if (pLayerInObj->Check_Name(tagName))
		{
			vPlayerDestPos = pLayerInObj->Get_TransformPtr()->Get_State(STATE::POS);
			vPlayerDestLook = XMVector3Normalize(pLayerInObj->Get_TransformPtr()->Get_State(STATE::LOOK));
		}
	}
	_vector vSrcPos = (pSrcTrans->Get_State(STATE::POS));
	_vector vTargetDir = XMVector3Normalize(vPlayerDestPos- vSrcPos);
	vSrcPos -= vTargetDir * 4.f;
	vTargetDir = XMVector3Normalize(vPlayerDestPos- vSrcPos);
	_float fTargetDis =  XMVectorGetX(XMVector3Length(vPlayerDestPos - vSrcPos));
	
	
	_float fDisMax = FLT_MAX;
	shared_ptr<CGameObject> pObj{};
	for (auto& pLayerDestPos : ComPareLayer->Get_ObjectList())
	{
		auto  pDestTransform = pLayerDestPos->Get_TransformPtr();
		_vector vDestPos = pDestTransform->Get_State(STATE::POS);

		if (XMVectorGetX(XMVector3Length(vDestPos - vSrcPos)) > fTargetDis )
			continue;
		
		_float3 vDestMax = pDestTransform->Get_Max();
		_float3 vDestMin = pDestTransform->Get_Min();
		
		if (vDestMax.y  < 25.f)
			continue;

		_matrix vDestInverseWorld = XMMatrixInverse(nullptr,pDestTransform->Get_World());

		_vector vLocalDestMax = XMLoadFloat3(&vDestMax);
		_vector vLocalDestMin = XMLoadFloat3(&vDestMin);
		
		
		_vector vCenter = (vLocalDestMax + vLocalDestMin) * 0.5f;
		_vector vExtents = (vLocalDestMax - vLocalDestMin) * 0.5f;

		_vector vSrcLocalPos = XMVector3TransformCoord(vSrcPos, vDestInverseWorld);
		_vector vTargetLocalDir= XMVector3Normalize(XMVector3TransformNormal(vTargetDir, vDestInverseWorld));

		BoundingBox vDestBox{};
		XMStoreFloat3(&vDestBox.Center, vCenter);
		XMStoreFloat3(&vDestBox.Extents, vExtents);
		
		_float fDis{};
		if (vDestBox.Intersects(vSrcLocalPos, vTargetLocalDir, fDis))
		{
			if (fDis < 1.f)
				continue;
			if (fDisMax > fDis)
			{
				fDisMax = fDis;
				pObj = pLayerDestPos;
			}
		}
	}
	
	if (fDisMax == FLT_MAX)
	{
		if (XMVectorGetX(XMVector3Dot(vPlayerDestLook, vTargetDir)) > 0)
			return false;
	}
	else
	{
		//if (CheckMesh_Triangle(pObj, pObj->Get_MeshIndexList(), vSrcPos, vTargetDir, nullptr))
			return false;
		
	}


	
		
	return true;
}

void CCollisionManager::RayCalculator(class CTransform* pSrcTrasnform, class CTransform* pDstTransform, _vector& SrcPos,  _vector& vCenter,_vector& Extern, _hvector offset)
{
	_matrix Dstworld = pDstTransform->Get_World();
	_vector DstPos = Dstworld.r[3];
	_float3 DstMax = pDstTransform->Get_Max();
	_float3 DstMin = pDstTransform->Get_Min();

	vCenter = (XMLoadFloat3(&DstMax) + XMLoadFloat3(&DstMin)) * 0.5f;
	vCenter = XMVector3TransformCoord(vCenter, Dstworld);
	XMStoreFloat3(&DstMax, XMVector3TransformCoord(XMLoadFloat3(&DstMax), Dstworld));
	Extern = vCenter - XMLoadFloat3(&DstMax);

	_matrix Srcworld = pSrcTrasnform->Get_World();
	SrcPos = XMVectorSetW(Srcworld.r[3] + offset,1.f);

}

CGameObject* CCollisionManager::AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, weak_ptr<CGameObject> pObj, _bool bBack,_float4* fOutPos)
{
	auto SrcObj = pObj.lock();
	if (NULL_TRUE(SrcObj))
		return nullptr;
	_vector SrcPos = SrcObj->Get_TransformPtr()->Get_State(STATE::POS);
	_vector SrcDir = XMVector3Normalize(SrcObj->Get_TransformPtr()->Get_State(STATE::LOOK));
	auto SrcTransform = SrcObj->Get_Transform();
	CLayer* pLayer = nullptr;
	for (size_t i = 0; i < endLayerIndex; ++i)
	{
		pLayer = CGameInstance::Get().Find_Layer(i, LayerName);
		if (pLayer != nullptr)
			break;
	}

	if (NULL_TRUE(pLayer))
		return nullptr;

	for (auto& pDest : pLayer->Get_ObjectList())
	{		
		if (pDest->Get_EndObject())
			continue;

		//여긴덷 
		
		auto DestTransform = pDest->Get_TransformPtr();
		_vector DestPos = DestTransform->Get_State(STATE::POS);
		
		if (XMVectorGetX(XMVector3Length(SrcPos - DestPos)) > 50.f)
			continue;

			if(Only_AABB_Collision(SrcTransform, pDest->Get_Transform()))
			if (CheckMesh_Triangle(pDest, pDest->Get_MeshIndexList(), SrcPos, SrcDir, fOutPos))
				return pDest.get();
		
			
	}

	return nullptr;

}
_bool CCollisionManager::Only_AABB_Collision(CTransform* pSrcTransform, _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix, vector<GRAB_ARM_EDGE>& EdgePoses)
{
	auto   SrcTransform = pSrcTransform;

	_matrix SrcWorld = SrcTransform->Get_World();
	_vector SrcWorldPos = SrcTransform->Get_State(STATE::POS);
	_vector vUp = SrcTransform->Get_State(STATE::UP);
	_matrix InverseSrcWorld = XMMatrixInverse(nullptr, SrcWorld);
	_vector Test = SrcWorld.r[3];
	//Ray 시작점
	//현재 직선손 직선구간에 ray 쏴서 걸리는 물체가 있는지 확인
	_vector startorigin = XMVectorSetW(startmat, 1.f);
	_vector startPos = {};
	_vector LastendPos = XMVectorSetW(endMat, 1.f);

	_vector SrcPos = SrcWorld.r[3];
	//_vector actualStart = EdgePoses.empty() ? startPos : XMLoadFloat3(&EdgePoses.back());
	//에라 모르겠다 로컬에서 비교~
	LastendPos = XMVector3TransformCoord(LastendPos, InverseSrcWorld);
	startPos = XMVector3TransformCoord(startorigin, InverseSrcWorld);
	_vector RayDir{};
	RayDir = XMVector3Normalize(LastendPos - startPos);
	_float fDist{};
	BoundingBox box;
	BoundingBox WorldBox;
	_float3 SrcMin = SrcTransform->Get_Min();
	_float3 SrcMax = SrcTransform->Get_Max();

	XMStoreFloat3(&box.Center, (XMLoadFloat3(&SrcMin) + XMLoadFloat3(&SrcMax)) * 0.5f);
	XMStoreFloat3(&box.Extents, (XMLoadFloat3(&SrcMax) - XMLoadFloat3(&SrcMin)) * 0.5f);

	if (box.Intersects(startPos, RayDir, fDist))
	{

		//위치만큼 이동한 점 찾고
		_vector WorldHitPos = startPos + (RayDir * fDist);

		_float SrcLength = XMVectorGetX(XMVector3Length(LastendPos - startPos));

		_vector offsetDir = XMVector3Normalize(WorldHitPos - XMLoadFloat3(&WorldBox.Center));
		WorldHitPos += offsetDir * 0.8f;

		_float WordDis = XMVectorGetX(XMVector3Length(WorldHitPos - startPos));

		if (fDist <= SrcLength)
		{
			_float3 LastPos{};
			XMStoreFloat3(&LastPos, XMVector3TransformCoord(WorldHitPos, SrcWorld));
			if (!EdgePoses.empty())
			{
				_vector ObjectWorld = XMVector3TransformCoord(XMLoadFloat3(&box.Center), SrcWorld);
				_float PlayerToObjectLength = XMVectorGetX(XMVector3Length(ObjectWorld - startorigin));
				_float Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&EdgePoses.back().fPos) - XMLoadFloat3(&LastPos)));
				if (Length < 3.f || PlayerToObjectLength < 3.f)
					return false;
			}

			GRAB_ARM_EDGE Edge{};
			_float3 Normal = {}; 

			XMStoreFloat3(&Normal,XMVector3Normalize(XMLoadFloat3(&LastPos) - SrcWorldPos ));

			
			Edge.fPos = LastPos;
			Edge.fNormal[0] = {-Normal.z , 0, Normal.x};
			_float fArrow = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Edge.fNormal[0]),XMVector3Normalize(XMVectorSetY(OriginMatrix.r[3],0.f) - XMVectorSetY(XMLoadFloat3(&LastPos),0.f))));
			if (fArrow < 0)
				Edge.m_bLeft = true;
			else Edge.m_bLeft = false;
			EdgePoses.push_back(Edge);
			return true;
		}
		//로컬 공간에서의 충돌 지점 계산


	}
	return false;

}
_bool CCollisionManager::AABB_CheckinLayer(const uint32_t endLayerIndex, const _wstring LayerName, _vector readStart, _vector startmat, _fvector endMat, _cmatrix OriginMatrix , vector<GRAB_ARM_EDGE>& EdgePoses, vector<uint32_t>& iSizecnt,_bool bFinished, _bool bCheck)
{
	_bool bEnd = false;
	CLayer* pLayer = nullptr;

	for (size_t i = 0; i < endLayerIndex; ++i)
	{
		pLayer = CGameInstance::Get().Find_Layer(i, LayerName);
		if (pLayer != nullptr)
			break;
	}

	if (NULL_TRUE(pLayer))
		return false;

	if (bFinished)
	{
		if (EdgePoses.empty())
			return {};
		if (!bCheck)
		{

			_float Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&EdgePoses.front().fPos) - readStart));
			if (Length < 1.5f)
			{
				EdgePoses.erase(EdgePoses.begin());
				return {};
			}
		}
		else
		{

			_float Length = XMVectorGetX(XMVector3Length(XMLoadFloat3(&EdgePoses.back().fPos) - readStart));
			if (Length < 1.5f)
			{
				EdgePoses.pop_back();
				return {};
			}
		}
		
		return {};
	}
	uint32_t iCnt{};
	_float offset = {8.f};
	if (bFinished)
		offset = -1.f;
	_float   MaxDist{ 0 };
	if (!EdgePoses.empty() && !bCheck)
	{
		_float3 Edge{}, EdgeNormalUp;
			Edge = EdgePoses.back().fPos;
			EdgeNormalUp = EdgePoses.back().fNormal[0];
		_float fEdgeDist{};
		_bool bDel{ false };
		_vector vLook = XMVector3Normalize(XMVectorSetY(OriginMatrix.r[3],0.f) - XMVectorSetY(XMLoadFloat3(&Edge),0.f));

		if (EdgePoses.back().m_bLeft)
		{
			_float DOt = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&EdgeNormalUp), vLook));
			if(DOt > 0 )
				bDel =true;
		}
		else
		{
			_float DOt = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&EdgeNormalUp), vLook));
			if (DOt < 0)
				bDel = true;
		}
		if (!bDel && !bCheck)
		{
			
			EdgePoses.pop_back();
			return false;
			
		}
	
	}
	if (bFinished)
		return false;

	for (auto& pObj : pLayer->Get_ObjectList())
	{
		if (Only_AABB_Collision(pObj->Get_TransformPtr(), readStart, startmat, endMat, OriginMatrix, EdgePoses))
		{
			auto pTrigger = pObj->Find_Component(L"Trigger");
			EdgePoses.back().bCheck = false;
			if (static_pointer_cast<CTrigger>(pTrigger)->Check_Trigger_Event(TRIGGER_EVENT::ELECTRICPOLE))
			{
				EdgePoses.back().bCheck = true;
				CGameInstance::Get().Add_Check_Collision(COLLISION::TRIGGER, pObj);
				iSizecnt.push_back(EdgePoses.size() - 1);

			}
		}
		
			
	}

	
	
	return true;

}

_bool CCollisionManager::CheckMesh_Triangle(shared_ptr<CGameObject> pObj, const vector<uint32_t>& MeshNumbers, _fvector vOriginPos, _fvector vOriginDir,_float4* OutPos)
{

	auto pTransform = pObj->Get_Transform().lock();
	_matrix pSrcWorld = pTransform->Get_World();

	_matrix InverseMatrix = XMMatrixInverse(nullptr,pTransform->Get_World());
	_float fMax = FLT_MAX;
	_vector vPos = XMVectorSet(0,0,0,1);
	_bool		bFinished{ false };

	_vector vLocalPos = XMVector3TransformCoord(vOriginPos, InverseMatrix);
	_vector vLocalDir = XMVector3Normalize(XMVector3TransformNormal(vOriginDir, InverseMatrix));

	for (auto& iter : MeshNumbers)
	{
		auto Vertices = CGameInstance::Get().Get_MeshVetexesLists(iter);
		
		auto Indices = CGameInstance::Get().Get_MeshIndicesLists(iter);
		if (NULL_TRUE(Vertices) || NULL_TRUE(Indices))
			return false;

		uint32_t index{ 0 };
		for (size_t i = 0; i < Indices->size(); i+=3)
		{
			_vector V0 = XMLoadFloat3(&(*Vertices)[(*Indices)[i]].fPos) ;
			_vector V1 = XMLoadFloat3(&(*Vertices)[(*Indices)[i + 1]].fPos);
			_vector V2 = XMLoadFloat3(&(*Vertices)[(*Indices)[i + 2]].fPos);
			_float fDist{};


			if (TriangleTests::Intersects(vLocalPos, vLocalDir,V0, V1, V2, fDist))
			{
				if (fMax > fDist)
				{
					if (NULL_TRUE(OutPos))
						return true;
					vPos = vLocalPos + vLocalDir * fDist;
					fMax = fDist;
					bFinished = true;
				}
			}

		}
	
	}

	if (NULL_FALSE(OutPos))
	{
		vPos = XMVector4Transform(vPos, pSrcWorld);
		XMStoreFloat4(OutPos, vPos);
	}
	
	return bFinished;
}

weak_ptr<CGameObject> CCollisionManager::Matrix_Check_Collision(_fmatrix Checck, COLLISION eCollisionValue)
{
	
	auto Collision = m_CollisionCheckList.find(eCollisionValue);

	if (Collision != m_CollisionCheckList.end())
	{
		
		for (auto& coll : Collision->second)
		{
			auto Src = coll.lock();
			if (NULL_TRUE(Src))
				continue;

			auto SrcTransform = Src->Get_Transform().lock();

			
			BoundingBox box;
			
			_float3 SrcLocalMax = SrcTransform->Get_Max();
			_float3 SrcLocalMin = SrcTransform->Get_Min();
			
			_matrix SrcInverseWorld = XMMatrixInverse(nullptr,SrcTransform->Get_World());
			_vector SrcCenter =  (XMLoadFloat3(&SrcLocalMax) + XMLoadFloat3(&SrcLocalMin)) * 0.5f;
			_vector SrcExtents = (XMLoadFloat3(&SrcLocalMax) - XMLoadFloat3(&SrcLocalMin)) * 0.5f;
			
			_vector DstPos = Checck.r[3];
			
			DstPos = XMVector3TransformCoord(DstPos, SrcInverseWorld);
			XMStoreFloat3(&box.Center ,SrcCenter);
			XMStoreFloat3(&box.Extents, SrcExtents);


			_vector Dir = XMVector3Normalize(SrcCenter - DstPos);
			_float Dist{};
			if (box.Intersects(DstPos, Dir, Dist))
			{

				if (Dist < 10.f)
				{
					return Src;
				}
			}
		}
			
	}

	return {};
}

unique_ptr<CCollisionManager> CCollisionManager::Create()
{
	return unique_ptr<CCollisionManager>(new CCollisionManager);
}


//for (int i = 0; i < 3; ++i)
//{
//	memcpy(vPlayerAxis[i], vPlayerWorld.m[i], sizeof(_vec3));  //obb 비교할 대상의 축 3개 추출
//	memcpy(vMonsterAxis[i], vMonsterWorld.m[i], sizeof(_vec3));//obb 비교할 대상의 축 3개 추출
//	D3DXVec3Normalize(&vPlayerAxis[i], &vPlayerAxis[i]);
//	D3DXVec3Normalize(&vMonsterAxis[i], &vMonsterAxis[i]);
//}
//for (int i = 0; i < 3; ++i)
//{
//	_float fDot = fabsf(D3DXVec3Dot(&vDistance, &vPlayerAxis[i])); //중심 거리 벡터를 현재 플레이어 축에서 투영한 길이
//	_float rA, rB;												   //현재 중심 - 중심의 길이와 축 하나를 Dot할 경우 해당 축기준으로 바라본 중심의 길이가 나옴
//
//	rA = 2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[0], &vPlayerAxis[i])) +   //
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[1], &vPlayerAxis[i])) +    // right up look을 을 각각 비교 축 기준으로 비교하여 더한 길이의 값
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[2], &vPlayerAxis[i]));     //
//
//	rB = 2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[0], &vPlayerAxis[i])) +
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[1], &vPlayerAxis[i])) +
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[2], &vPlayerAxis[i]));
//
//	fBack = rA + rB - fDot;
//	if ((fDot > fabsf(rA + rB)))
//	{
//		m_bTrue = true;
//		break;
//	}
//	else
//	{
//		if (fBack < fMin)// 제일거리 짧은거 기준으로 밀어내기
//		{
//			vAxis = vPlayerAxis[i]; //그떄 충돌했던 축을 담아놓기
//			fMin = fBack; // 최소값저장
//		}
//	}
//
//}
//
//for (int i = 0; i < 3; ++i)
//{
//	_float fDot = fabsf(D3DXVec3Dot(&vDistance, &vMonsterAxis[i])); //대상과의 거리와 현재 몬스터 축 하나를 내적
//	_float rA, rB;
//
//	rA = 2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[0], &vMonsterAxis[i])) +   //몬스터 축기준으로 비교
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[1], &vMonsterAxis[i])) +
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[2], &vMonsterAxis[i]));
//
//	rB = 2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[0], &vMonsterAxis[i])) +
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[1], &vMonsterAxis[i])) +
//		2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[2], &vMonsterAxis[i]));
//
//	fBack = rA + rB - fDot;
//
//	if ((fDot > fabsf(rA + rB)))
//	{
//		m_bTrue = true;
//		break;
//	}
//	else
//	{
//		if (fBack < fMin)
//		{
//			vAxis = vMonsterAxis[i];
//			fMin = fBack;
//		}
//	}
//}
//
//
//for (int i = 0; i < 3; ++i)
//{
//	for (int j = 0; j < 3; ++j)
//	{
//		_vec3 vCross;
//		D3DXVec3Cross(&vCross, &vPlayerAxis[i], &vMonsterAxis[j]);
//
//		if (D3DXVec3LengthSq(&vCross) < 0.0001f);
//		continue;
//		D3DXVec3Normalize(&vCross, &vCross);
//
//		_float fDot = fabsf(D3DXVec3Dot(&vDistance, &vCross)); //대상과의 거리와 현재 몬스터 축 하나를 외적
//		_float rA, rB;
//
//		rA = 2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[0], &vCross)) +   //플레이어의 축기준으로 비교
//			2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[1], &vCross)) +
//			2 * 0.5f * fabsf(D3DXVec3Dot(&vPlayerAxis[2], &vCross));
//
//		rB = 2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[0], &vCross)) +
//			2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[1], &vCross)) +
//			2 * 0.5f * fabsf(D3DXVec3Dot(&vMonsterAxis[2], &vCross));
//
//
//
//		fBack = rA + rB - fDot;
//
//		if ((fDot > fabsf(rA + rB)))
//		{
//			m_bTrue = true;
//			break;
//		}
//		else
//		{
//			if (fBack < fMin)
//			{
//				vAxis = vCross;
//				fMin = fBack;
//			}
//		}
//	}
//}
//
//if (!m_bTrue)
//{
//	D3DXVec3Normalize(&vAxis, &vAxis);
//
//	if (D3DXVec3Dot(&vDistance, &vAxis) < 0.f)
//		vAxis *= -1.f;
//
//	fMin += 0.001f;
//	vAxis = vAxis * fMin;
//
//	vPlayerPos -= vAxis;
//
//	m_bJump = false;
//	m_bFalling = false;
//}