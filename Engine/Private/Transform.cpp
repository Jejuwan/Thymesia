#include "Engine_Pch.h"
#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	
}

_vector CTransform::Get_State(STATE eState)
{
	/* float2344x4 -> vector or matrix : Load */
	/* vector or matrix -> float2344x4   : Store */
	
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return WorldMatrix.r[eState];	
}

_float3 CTransform::Get_Scaled()
{
	return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
}


void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if(nullptr != pArg)
		memmove(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);	
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Dir(_float3 _vDir, _float fSpeed, _float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDir = XMLoadFloat3(&_vDir);

	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * fSpeed * fTimeDelta;
	
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Pos(_float3 _vPos,_float fSpeed, _float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vPos = XMLoadFloat3(&_vPos);

	vPosition -= vPos* fSpeed* fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Scaled(const _float3 & vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Chase(_fvector vTargetPos,_float fSpeed, _float fTimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vLook));

	if(fDistance >= fLimit)
		vPosition += XMVector3Normalize(vLook) * fSpeed * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	Scaled(vScaled);
}

void CTransform::LookAtDir(_fvector lookDir)
{
	_vector		vLook = lookDir;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

	Scaled(vScaled);
}

void CTransform::CopyRotation(CTransform* pTrans)
{
	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, pTrans->Get_State(CTransform::STATE_RIGHT));
	Set_State(STATE_UP, pTrans->Get_State(CTransform::STATE_UP));
	Set_State(STATE_LOOK, pTrans->Get_State(CTransform::STATE_LOOK));

	Scaled(vScaled);
}

_float CTransform::Distance(CTransform* trans)
{
	return XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_POSITION) - trans->Get_State(CTransform::STATE_POSITION)));
}

_vector CTransform::Get_HoriziontalLookAtDir(CTransform* trans)
{
	_vector vDir = trans->Get_State(CTransform::STATE_POSITION) - Get_State(CTransform::STATE_POSITION);
	_float3 fDir;
	XMStoreFloat3(&fDir, vDir);
	fDir.y = 0.f;
	XMStoreFloat3(&fDir,XMVector3Normalize(XMLoadFloat3(&fDir)));

	return XMLoadFloat3(&fDir);
}

_vector CTransform::Get_HoriziontalLookDir()
{
	_vector vLook = Get_State(CTransform::STATE_LOOK);
	_float4 fLook;
	XMStoreFloat4(&fLook, vLook);
	fLook.y = 0.f;

	return XMLoadFloat4(&fLook);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTransform");
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTransform");
	}

	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
