#include "Engine_Pch.h"
#include "..\Public\VIBuffer_Cube.h"
#include "GameInstance.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
	: CVIBuffer(rhs),
	m_pTriangleMesh(rhs.m_pTriangleMesh)
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 24;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumIndices = 36;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXNORTEX*		pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);
	pVertices[0].vNormal = _float3(0.f, 0.f,-1.f);
	pVertices[1].vNormal = _float3(0.f, 0.f,-1.f);
	pVertices[2].vNormal = _float3(0.f, 0.f,-1.f);
	pVertices[3].vNormal = _float3(0.f, 0.f,-1.f);

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[5].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[6].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[4].vTexcoord = _float2(0.f, 0.f);
	pVertices[5].vTexcoord = _float2(1.f, 0.f);
	pVertices[6].vTexcoord = _float2(1.f, 1.f);
	pVertices[7].vTexcoord = _float2(0.f, 1.f);
	pVertices[4].vNormal = _float3(-1.f,0.f,0.f);
	pVertices[5].vNormal = _float3(-1.f,0.f,0.f);
	pVertices[6].vNormal = _float3(-1.f,0.f,0.f);
	pVertices[7].vNormal = _float3(-1.f,0.f,0.f);

	pVertices[8].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[9].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[10].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[11].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[8].vTexcoord = _float2(0.f, 0.f);
	pVertices[9].vTexcoord = _float2(1.f, 0.f);
	pVertices[10].vTexcoord = _float2(1.f, 1.f);
	pVertices[11].vTexcoord = _float2(0.f, 1.f);
	pVertices[8].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[9].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[10].vNormal = _float3(0.f, 0.f, 1.f);
	pVertices[11].vNormal = _float3(0.f, 0.f, 1.f);

	pVertices[12].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[13].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[14].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[15].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[12].vTexcoord = _float2(0.f, 0.f);
	pVertices[13].vTexcoord = _float2(1.f, 0.f);
	pVertices[14].vTexcoord = _float2(1.f, 1.f);
	pVertices[15].vTexcoord = _float2(0.f, 1.f);
	pVertices[12].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[13].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[14].vNormal = _float3(1.f, 0.f, 0.f);
	pVertices[15].vNormal = _float3(1.f, 0.f, 0.f);

	pVertices[16].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[17].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[18].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[19].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[16].vTexcoord = _float2(0.f, 0.f);
	pVertices[17].vTexcoord = _float2(1.f, 0.f);
	pVertices[18].vTexcoord = _float2(1.f, 1.f);
	pVertices[19].vTexcoord = _float2(0.f, 1.f);
	pVertices[16].vNormal = _float3(0.f, 1.f, 0.f);
	pVertices[17].vNormal = _float3(0.f, 1.f, 0.f);
	pVertices[18].vNormal = _float3(0.f, 1.f, 0.f);
	pVertices[19].vNormal = _float3(0.f, 1.f, 0.f);

	pVertices[20].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[21].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[22].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[23].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[20].vTexcoord = _float2(0.f, 0.f);
	pVertices[21].vTexcoord = _float2(1.f, 0.f);
	pVertices[22].vTexcoord = _float2(1.f, 1.f);
	pVertices[23].vTexcoord = _float2(0.f, 1.f);
	pVertices[20].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[21].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[22].vNormal = _float3(0.f, -1.f, 0.f);
	pVertices[23].vNormal = _float3(0.f, -1.f, 0.f);

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

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

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	pIndices[6] = 4;
	pIndices[7] = 5;
	pIndices[8] = 6;
	pIndices[9] = 4;
	pIndices[10] = 6;
	pIndices[11] = 7;

	pIndices[12] = 8;
	pIndices[13] = 9;
	pIndices[14] = 10;
	pIndices[15] = 8;
	pIndices[16] = 10;
	pIndices[17] = 11;

	pIndices[18] = 12;
	pIndices[19] = 13;
	pIndices[20] = 14;
	pIndices[21] = 12;
	pIndices[22] = 14;
	pIndices[23] = 15;

	pIndices[24] = 16;
	pIndices[25] = 17;
	pIndices[26] = 18;
	pIndices[27] = 16;
	pIndices[28] = 18;
	pIndices[29] = 19;

	pIndices[30] = 20;
	pIndices[31] = 21;
	pIndices[32] = 22;
	pIndices[33] = 20;
	pIndices[34] = 22;
	pIndices[35] = 23;

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	PxVec3* c = new PxVec3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; i++)
		c[i] = { pVertices[i].vPosition.x,pVertices[i].vPosition.y,pVertices[i].vPosition.z };

	PxU32* d = new PxU32[m_iNumIndices];
	for (_uint i = 0; i < m_iNumIndices; i++)
		d[i] = pIndices[i];

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = m_iNumVertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = c;

	meshDesc.triangles.count = m_iNumIndices / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = d;

	//meshDesc.flags = PxMeshFlags(0);

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status)
		return E_FAIL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	m_pTriangleMesh = pGameInstance->GetPhysics()->createTriangleMesh(readBuffer);

	Safe_Release(pGameInstance);
	Safe_Delete_Array(c);
	Safe_Delete_Array(d);
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
	}

	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
