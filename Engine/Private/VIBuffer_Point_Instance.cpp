#include "Engine_Pch.h"
#include "..\Public\VIBuffer_Point_Instance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumInstance = iNumInstance;
	m_iNumIndicesPerInstance = 1;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iStride = sizeof(VTXPOSPSIZE);
	m_iNumIndices = m_iNumIndicesPerInstance * m_iNumInstance;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOSPSIZE*		pVertices = new VTXPOSPSIZE[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSPSIZE) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.f,1.f);

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);	

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memmove(&m_InstanceData, pArg, sizeof m_InstanceData);

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	VTXPOINTINSTANCE*	pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	random_device		RandomDevice;

	/* 난수 생성 초기화.  */
	m_RandomNumber = mt19937_64(RandomDevice());
	m_RandomX = uniform_real_distribution<float>(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	m_RandomY = uniform_real_distribution<float>(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	m_RandomZ = uniform_real_distribution<float>(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
	uniform_real_distribution<float> randomDir = uniform_real_distribution<float>(0.f,3.14f);
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vRight = { 1,0,0,0 };
		pInstanceVertices[i].vUp = { 0,1,0,0 };
		pInstanceVertices[i].vLook = { 0,0,1,0 };
		_float4 v = {m_InstanceData.vPivotPos.x + m_RandomX(m_RandomNumber),
			m_InstanceData.vPivotPos.y + m_RandomY(m_RandomNumber),
			m_InstanceData.vPivotPos.z + m_RandomZ(m_RandomNumber), 0 };
		//_matrix m = XMMatrixRotationQuaternion(XMQuaternionRotationAxis({ 0,1,0 }, 0.5f)); 
		//XMStoreFloat4(&pInstanceVertices[i].vRight,m.r[0]);
		//XMStoreFloat4(&pInstanceVertices[i].vUp, m.r[1]);
		//XMStoreFloat4(&pInstanceVertices[i].vLook, m.r[2]);
		mt19937_64 rannum = mt19937_64(RandomDevice());
		pInstanceVertices[i].vTranslation = _float4(v.x,v.y,v.z, 1.f);
		pInstanceVertices[i].vRotDir = _float2(randomDir(rannum), randomDir(rannum));
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pInstanceVertices;

	if (FAILED(__super::Create_Buffer(&m_pInstanceVB)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	uniform_real_distribution<float>	RandomSpeedX(m_InstanceData.fMinDir.x, m_InstanceData.fMaxDir.x);
	uniform_real_distribution<float>	RandomSpeedY(m_InstanceData.fMinDir.y, m_InstanceData.fMaxDir.y);
	uniform_real_distribution<float>	RandomSpeedZ(m_InstanceData.fMinDir.z, m_InstanceData.fMaxDir.z);
	uniform_real_distribution<float>	RandomSpeed(m_InstanceData.fSpeed.x, m_InstanceData.fSpeed.y);

	m_pInstanceSpeed = new _float3[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat3(&m_pInstanceSpeed[i],XMVector3Normalize({ RandomSpeedX(m_RandomNumber),RandomSpeedY(m_RandomNumber),RandomSpeedZ(m_RandomNumber) })* RandomSpeed(m_RandomNumber));
	}

	m_InstanceData.fLifeTime = 0.f;

	return S_OK;
}

void CVIBuffer_Point_Instance::Tick(_float fTimeDelta)
{
	m_InstanceData.fLifeTime += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	
	m_pContext->Map(m_pInstanceVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		((VTXPOINTINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pInstanceSpeed[i].x * fTimeDelta;
		((VTXPOINTINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pInstanceSpeed[i].y * fTimeDelta;
		((VTXPOINTINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pInstanceSpeed[i].z * fTimeDelta;
	/*	if (0 > ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y)
			((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y = m_InstanceData.vPivotPos.y + m_RandomY(m_RandomNumber);*/
		_float fRelativeFallSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pInstanceSpeed[i]))) * m_InstanceData.fRelativeFallSpeed;
		if (fRelativeFallSpeed == 0.f)
			fRelativeFallSpeed = 1.f;
		((VTXPOINTINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_InstanceData.fGravityForce * m_InstanceData.fGravityForce * fRelativeFallSpeed * m_InstanceData.fLifeTime;
	}

	m_pContext->Unmap(m_pInstanceVB, 0);
}

CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance");
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance");
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pInstanceSpeed);
}
