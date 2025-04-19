#include "Engine_Pch.h"
#include "..\Public\Camera.h"
#include "Component_Manager.h"

#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return	S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CAMERADESC*		pCameraDesc = (CAMERADESC*)pArg;

	CComponent_Manager*	pComponent_Manager = CComponent_Manager::Get_Instance();
	Safe_AddRef(pComponent_Manager);

	m_pTransform = (CTransform*)pComponent_Manager->Clone_Component(pCameraDesc->iLevelIndex, pCameraDesc->strTranformTag, &pCameraDesc->TransformDesc);
	if (nullptr == m_pTransform)
		return E_FAIL;

	Safe_Release(pComponent_Manager);

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransform->LookAt(XMLoadFloat4(&pCameraDesc->vAt));
	
	m_fFovy = pCameraDesc->fFovy;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	m_pTransform->LookAtDir({ 0,0,1 });
	return	S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}


HRESULT CCamera::Set_Transform()
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	_float4x4 shadowViewMat;
	XMStoreFloat4x4(&shadowViewMat, XMMatrixRotationX(XMConvertToRadians(15.f))*XMMatrixRotationY(XMConvertToRadians(-30.f)));
	shadowViewMat._41 =m_pTransform->Get_WorldMatrix()._41 + 15;
	shadowViewMat._42 =10;
	shadowViewMat._43 =m_pTransform->Get_WorldMatrix()._43 - 15;

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_LIGHTVIEW,
		XMMatrixInverse(nullptr,XMLoadFloat4x4(&shadowViewMat)));

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, 
		m_pTransform->Get_WorldMatrix_Inverse());
	
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, 
		XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);
}
