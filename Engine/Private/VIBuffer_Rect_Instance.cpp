#include "Engine_Pch.h"
#include "..\Public\VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumInstance = iNumInstance;
	m_iNumIndicesPerInstance = 6;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXPOSTEX);
	m_iNumIndices = m_iNumIndicesPerInstance * m_iNumInstance;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iInstanceStride = sizeof(VTXRECTINSTANCE);

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOSTEX*		pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.1f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.1f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.1f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.1f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

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

	_uint			iNumIndices = 0;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void * pArg)
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

	VTXRECTINSTANCE*	pInstanceVertices = new VTXRECTINSTANCE[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXRECTINSTANCE) * m_iNumInstance);

	uniform_real_distribution<float>	RandomSpeedX(m_InstanceData.fMinDir.x, m_InstanceData.fMaxDir.x);
	uniform_real_distribution<float>	RandomSpeedY(m_InstanceData.fMinDir.y, m_InstanceData.fMaxDir.y);
	uniform_real_distribution<float>	RandomSpeedZ(m_InstanceData.fMinDir.z, m_InstanceData.fMaxDir.z);
	uniform_real_distribution<float>	RandomSpeed(m_InstanceData.fSpeed.x, m_InstanceData.fSpeed.y);

	m_pInstanceSpeed = new _float3[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat3(&m_pInstanceSpeed[i], XMVector3Normalize({ RandomSpeedX(m_RandomNumber),RandomSpeedY(m_RandomNumber),RandomSpeedZ(m_RandomNumber) }) * RandomSpeed(m_RandomNumber));
	}


	random_device		RandomDevice;

	/* 난수 생성 초기화.  */
	m_RandomNumber = mt19937_64(RandomDevice());
	m_RandomX = uniform_real_distribution<float>(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	m_RandomY = uniform_real_distribution<float>(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	m_RandomZ = uniform_real_distribution<float>(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
	
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vTranslation = _float4(m_InstanceData.vPivotPos.x + m_RandomX(m_RandomNumber),
			m_InstanceData.vPivotPos.y + m_RandomY(m_RandomNumber),
			m_InstanceData.vPivotPos.z + m_RandomZ(m_RandomNumber), 1.f);

		_float3 tempSpeed = m_pInstanceSpeed[i];
		tempSpeed.z = 0.f;
		float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&tempSpeed)),
			{ 0,1,0 ,0 })) * 3.14f/4 + 3.14f/4;
		//_matrix m = XMMatrixRotationQuaternion(XMQuaternionRotationAxis({ 0,0,1 }, fAngle));
		//XMStoreFloat4(&pInstanceVertices[i].vRight, m.r[0]);
		//XMStoreFloat4(&pInstanceVertices[i].vUp, m.r[1]);
		//XMStoreFloat4(&pInstanceVertices[i].vLook, m.r[2]);

		/*_matrix mat = XMMatrixScaling(0.1f, 1.f, 1.f);
		XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pInstanceVertices[i].vRight), mat));
		XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pInstanceVertices[i].vUp), mat));
		XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pInstanceVertices[i].vLook), mat));*/
	}

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pInstanceVertices;

	if (FAILED(__super::Create_Buffer(&m_pInstanceVB)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion


	m_InstanceData.fLifeTime = 0.f;

	return S_OK;
}

void CVIBuffer_Rect_Instance::Tick(_float fTimeDelta)
{
	m_InstanceData.fLifeTime += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	
	m_pContext->Map(m_pInstanceVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//_vector vRight = XMVector3Normalize(XMLoadFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vRight));
		//_vector vUp = XMVector3Normalize(XMLoadFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vUp));
		//_vector vLook = XMVector3Normalize(XMLoadFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vLook));

		//XMStoreFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vRight,vRight );
		//XMStoreFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vUp, vUp*0.1f);
		//XMStoreFloat4(&((VTXRECTINSTANCE*)SubResource.pData)[i].vLook, vLook );

		((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pInstanceSpeed[i].x * fTimeDelta;
		((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pInstanceSpeed[i].y * fTimeDelta;
		((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pInstanceSpeed[i].z * fTimeDelta;
		/*	if (0 > ((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y)
				((VTXRECTINSTANCE*)SubResource.pData)[i].vTranslation.y = m_InstanceData.vPivotPos.y + m_RandomY(m_RandomNumber);*/
		/*_float fRelativeFallSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pInstanceSpeed[i]))) * m_InstanceData.fRelativeFallSpeed;
		if (fRelativeFallSpeed == 0.f)
			fRelativeFallSpeed = 1.f;
		((VTXPOINTINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_InstanceData.fGravityForce * m_InstanceData.fGravityForce * fRelativeFallSpeed * m_InstanceData.fLifeTime;*/
		//((VTXRECTINSTANCE*)SubResource.pData)[i].vRotDir = { m_pInstanceSpeed[i].x,m_pInstanceSpeed[i].y,m_pInstanceSpeed[i].z,0 };
	}

	

	m_pContext->Unmap(m_pInstanceVB, 0);
}

CVIBuffer_Rect_Instance * CVIBuffer_Rect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CVIBuffer_Rect_Instance");
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_Instance::Clone(void * pArg)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pInstanceSpeed);
}
