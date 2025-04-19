#include "Engine_Pch.h"
#include "..\Public\Picking.h"
#include "Engine_Defines.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}


HRESULT CPicking::Initialize(ID3D11Device* pGraphic_Device, HWND hWnd)
{
	m_pGraphic_Device = pGraphic_Device;
	Safe_AddRef(m_pGraphic_Device);

	m_hWnd = hWnd;

	return S_OK;
}

void CPicking::Tick()
{
	POINT		ptMouse;

	GetCursorPos(&ptMouse);

	/* 뷰포트 상의 마우스 좌표를 구한다. */
	ScreenToClient(m_hWnd, &ptMouse);

	/* 투영스페이스 상의 마우스 좌표를 구한다. */
	_float3		vMousePos;
	/* x : 0 -> -1, WINSIZEX -> 1 */
	/* y : 0 -> 1, WINSIZEY -> -1 */
	RECT		rcClient;
	GetClientRect(m_hWnd, &rcClient);

	vMousePos.x = ptMouse.x / (rcClient.right * 0.5f) - 1.f;
	vMousePos.y = -ptMouse.y / (rcClient.bottom * 0.5f) + 1.f;
	vMousePos.z = 0.f; /* near평면을 클릭하였기 때문에. */

	/* 뷰스페이스상의 마우스 Ray, 마우스 RayPos을 구한다. */
	_float4x4		ProjMatrix;
	//CPipeLine::Get_Instance()->Set_Transform(CPipeLine::D3DTS_PROJ, XMLoadFloat4x4(&ProjMatrix));
	ProjMatrix = CPipeLine::Get_Instance()->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);

	/* 뷰스페이스 상의 마우스 위치. */
	XMStoreFloat3(&vMousePos,XMVector3TransformCoord(XMLoadFloat3(&vMousePos), XMLoadFloat4x4(&ProjMatrix)));
	m_vMouseRayPos = _float3(0.f, 0.f, 0.f);
	XMStoreFloat3(&m_vMouseRay,XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vMouseRayPos));

	/* 월드 스페이스 상의 마우스 Ray, RayPos */
	_float4x4		ViewMatrix;
	ViewMatrix = CPipeLine::Get_Instance()->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);

	XMStoreFloat3(&m_vMouseRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vMouseRayPos), XMLoadFloat4x4(&ViewMatrix)));
	XMStoreFloat3(&m_vMouseRay, XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), XMLoadFloat4x4(&ViewMatrix)));
	XMStoreFloat3(&m_vMouseRay, XMVector3Normalize(XMLoadFloat3(&m_vMouseRay)));
}

void CPicking::Transform_ToLocalSpace(const _float4x4& WorldMatrix)
{
	_float4x4		WorldMatrixInv;
	XMStoreFloat4x4(&WorldMatrixInv,XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat3(&m_vMouseRayPos_Local, XMVector3TransformCoord(XMLoadFloat3(&m_vMouseRayPos), XMLoadFloat4x4(&WorldMatrixInv)));
	XMStoreFloat3(&m_vMouseRayPos_Local, XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), XMLoadFloat4x4(&WorldMatrixInv)));

	XMStoreFloat3(&m_vMouseRayPos_Local, XMVector3Normalize(XMLoadFloat3(&m_vMouseRayPos_Local)));
}

void CPicking::Transform_ToLocalSpace_ForRay(const _float4x4& WorldMatrix, const _float3& _ray, const _float3& _rayPos, _float3* pOutVec, _float3* pOutPos)
{
	_float4x4		WorldMatrixInv;
	XMStoreFloat4x4(&WorldMatrixInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&WorldMatrix)));
	XMStoreFloat3(pOutPos, XMVector3TransformCoord(XMLoadFloat3(&_rayPos), XMLoadFloat4x4(&WorldMatrixInv)));
	XMStoreFloat3(pOutVec, XMVector3TransformNormal(XMLoadFloat3(&_ray), XMLoadFloat4x4(&WorldMatrixInv)));

	XMStoreFloat3(pOutVec, XMVector3Normalize(XMLoadFloat3(pOutVec)));

}

_bool CPicking::Intersect_Triangle_InWorld(const _float3* pPointA, const _float3* pPointB, const _float3* pPointC, _float3* pOut)
{
	_float	    fDist;

	_bool		isColl = { false };

	isColl = TriangleTests::Intersects(XMLoadFloat3(&m_vMouseRayPos), XMLoadFloat3(&m_vMouseRay), XMLoadFloat3(pPointA), XMLoadFloat3(pPointB), XMLoadFloat3(pPointC), fDist);

	if (true == isColl)
	{
		XMStoreFloat3(pOut, XMLoadFloat3(&m_vMouseRay) * XMLoadFloat(&fDist) + XMLoadFloat3(&m_vMouseRayPos));
	}

	return isColl;
}

_bool CPicking::Intersect_Triangle_InLocal(const _float3* pPointA, const _float3* pPointB, const _float3* pPointC, _float3* pOut)
{
	_float		fDist;

	_bool		isColl = { false };

	isColl = TriangleTests::Intersects(XMLoadFloat3(&m_vMouseRayPos), XMLoadFloat3(&m_vMouseRay), XMLoadFloat3(pPointA), XMLoadFloat3(pPointB), XMLoadFloat3(pPointC), fDist);

	if (true == isColl)
	{
		XMStoreFloat3(pOut, XMLoadFloat3(&m_vMouseRay_Local) * XMLoadFloat(&fDist) + XMLoadFloat3(&m_vMouseRayPos_Local));
	}

	return isColl;
}

_bool CPicking::Intersect_Triangle_ForRay(const _float3* pPointA, const _float3* pPointB, const _float3* pPointC, const _float3& _ray, const _float3& _rayPos, _float3* pOut)
{
	_float		fDist;

	_bool		isColl = { false };

	isColl = TriangleTests::Intersects(XMLoadFloat3(&m_vMouseRayPos), XMLoadFloat3(&m_vMouseRay), XMLoadFloat3(pPointA), XMLoadFloat3(pPointB), XMLoadFloat3(pPointC), fDist);

	if (true == isColl)
	{
		XMStoreFloat3(pOut, XMLoadFloat3(&_ray) * XMLoadFloat(&fDist) + XMLoadFloat3(&_rayPos));
	}
	else
	{
		int a = 1;
	}

	return isColl;
}

_bool CPicking::Intersect_Ray(BoundingOrientedBox* bSphere,_Out_ _float* pDist)
{
	m_vCursorNDC.x = m_CursorScreen.x * 2.0f / 1280 - 1.0f;
	m_vCursorNDC.y = -m_CursorScreen.y * 2.0f / 720 + 1.0f;

	// 커서가 화면 밖으로 나갔을 경우 범위 조절
	// 게임에서는 클램프를 안할 수도 있습니다.
	m_vCursorNDC.x = std::clamp(m_vCursorNDC.x, -1.0f, 1.0f);
	m_vCursorNDC.y = std::clamp(m_vCursorNDC.y, -1.0f, 1.0f);


	_matrix view = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix proj = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	_vector cursorNDCnear = { m_vCursorNDC.x, m_vCursorNDC.y, 0.f, 1.f };
	_vector cursorNDCfar = { m_vCursorNDC.x, m_vCursorNDC.y, 1.f, 1.f };

	_matrix invProjView = XMMatrixInverse(nullptr, proj) * XMMatrixInverse(nullptr, view);

	_vector cursorWorldNear = XMVector3TransformCoord(cursorNDCnear, invProjView);
	_vector cursorWorldFar = XMVector3TransformCoord(cursorNDCfar, invProjView);
	_vector dir = cursorWorldFar - cursorWorldNear;
	dir = XMVector3Normalize(dir);


	SimpleMath::Ray curRay = SimpleMath::Ray(cursorWorldNear, dir);
	float dist = 0.f;

	*pDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&(bSphere->Center)) - cursorWorldNear));

	return bSphere->Intersects(cursorWorldNear, dir, *pDist);
}

CGameObject* CPicking::Intersect_Ray_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = CGameInstance::Get_Instance()->Get_Layer(iLevelIndex, strLayerTag);

	if(pLayer==nullptr)
		return nullptr;

	_float fMinDist = 10000.f;
	class CGameObject* pPickedObj = nullptr;
	for (_uint i = 0; i < pLayer->size(); i++)
	{
		class CGameObject* pGameObj = CGameInstance::Get_Instance()->Get_GameObject(iLevelIndex, strLayerTag, i);
		_float fDist;
		if (Intersect_Ray(pGameObj->Get_Bounding_Box(), &fDist) && fDist < fMinDist)
		{
			fMinDist = fDist;
			pPickedObj = pGameObj;
		}
	}
	return pPickedObj;
}

_float3 CPicking::Get_Create_Pos(_float fDist)
{

	m_vCursorNDC.x =1280 / 2.f * 2.0f / 1280 - 1.0f;
	m_vCursorNDC.y = -720 / 2.f * 2.0f / 720 + 1.0f;

	// 커서가 화면 밖으로 나갔을 경우 범위 조절
	// 게임에서는 클램프를 안할 수도 있습니다.
	m_vCursorNDC.x = std::clamp(m_vCursorNDC.x, -1.0f, 1.0f);
	m_vCursorNDC.y = std::clamp(m_vCursorNDC.y, -1.0f, 1.0f);


	_matrix view = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix proj = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	_vector cursorNDCnear = { m_vCursorNDC.x, m_vCursorNDC.y, 0.f, 1.f };
	_vector cursorNDCfar = { m_vCursorNDC.x, m_vCursorNDC.y, 1.f, 1.f };

	_matrix invProjView = XMMatrixInverse(nullptr, proj) * XMMatrixInverse(nullptr, view);

	_vector cursorWorldNear = XMVector3TransformCoord(cursorNDCnear, invProjView);
	_vector cursorWorldFar = XMVector3TransformCoord(cursorNDCfar, invProjView);
	_vector dir = cursorWorldFar - cursorWorldNear;
	dir = XMVector3Normalize(dir);

	_float3 vPos;
	XMStoreFloat3(&vPos, cursorWorldNear + dir * fDist);
	return vPos;
}

void CPicking::Free()
{

}
