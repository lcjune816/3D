#include "CollisionManager.h"
#include "GameInstance.h"
#include "DirectXCollision.h"
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
	
	_float rayX = (2.f *  tagCollision.fMouse.x / 1280.f - 1.f) / Proj(0, 0);
	_float rayY = (-2.f * tagCollision.fMouse.y / 720.f  + 1.f) / Proj(1, 1);

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

	weak_ptr<CGameObject> pObj = {};
	_matrix InverseWorld = {};
	_vector Worldray = {}, WorldrayDir = {}, OriginRay = {}, OriginDir = {};

	for (auto& iter : pLayer->Get_ObjectList())
	{
		//월드로만 비교
		Worldray    = rayOrigin;
		WorldrayDir = rayDir;
		InverseWorld = XMMatrixInverse(nullptr, iter->Get_Transform()->Get_World());
		//대상의 월드 pos를 가져옴
		
		Worldray = XMVector3TransformCoord(Worldray, InverseWorld);
		WorldrayDir = XMVector3Normalize(XMVector3TransformNormal(WorldrayDir, InverseWorld));

		_float3 fMax = iter->Get_Transform()->Get_Max();
		_float3 fMin = iter->Get_Transform()->Get_Min();
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

	_float3 vMax = pObj->Get_Transform()->Get_Max();
	_float3 vMin = pObj->Get_Transform()->Get_Min();
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

_bool CCollisionManager::Only_AABB_Collision(const weak_ptr<CTransform> pSrcTransform, const weak_ptr<CTransform> pDstTransform)
{
	auto   SrcTransform = pSrcTransform.lock();
	auto   DstTransform = pDstTransform.lock();

	_matrix SrcWorld = SrcTransform->Get_World();
	_matrix DstWorld = DstTransform->Get_World();
	_float3  SrcMin, SrcMax;
	_float3  DstMin, DstMax;


	SrcMin = SrcTransform->Get_Min();
	SrcMax = SrcTransform->Get_Max();

	XMStoreFloat3(&SrcMin, XMVector3TransformCoord(XMLoadFloat3(&SrcMin), SrcWorld));
	XMStoreFloat3(&SrcMax, XMVector3TransformCoord(XMLoadFloat3(&SrcMax), SrcWorld));

	DstMin = DstTransform->Get_Min();
	DstMax = DstTransform->Get_Max();

	XMStoreFloat3(&DstMin, XMVector3TransformCoord(XMLoadFloat3(&DstMin), DstWorld));
	XMStoreFloat3(&DstMax, XMVector3TransformCoord(XMLoadFloat3(&DstMax), DstWorld));

	SrcMin.x = min(SrcMin.x, SrcMax.x);
	SrcMin.y = min(SrcMin.y, SrcMax.y);
	SrcMin.z = min(SrcMin.z, SrcMax.z);

	SrcMax.x = max(SrcMin.x, SrcMax.x);
	SrcMax.y = max(SrcMin.y, SrcMax.y);
	SrcMax.z = max(SrcMin.z, SrcMax.z);


	DstMin.x = min(DstMin.x, DstMax.x);
	DstMin.y = min(DstMin.y, DstMax.y);
	DstMin.z = min(DstMin.z, DstMax.z);

	DstMax.x = max(DstMin.x, DstMax.x);
	DstMax.y = max(DstMin.y, DstMax.y);
	DstMax.z = max(DstMin.z, DstMax.z);


	if (DstMax.x > SrcMin.x &&
		DstMax.y > SrcMin.y &&
		DstMax.z > SrcMin.z &&
		DstMin.x < SrcMax.x &&
		DstMin.y < SrcMax.y &&
		DstMin.z < SrcMax.z)
	{
		return true;
	}

	return false;
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