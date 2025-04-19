#include "Engine_Pch.h"
#include "..\Public\Mesh.h"
#include "GameInstance.h"
#include "Bone.h"
#include "File.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs),
	m_pTriangleMesh(rhs.m_pTriangleMesh)
{
}

void CMesh::Get_BoneMatrices(_float4x4* pBoneMatrices, const CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	_uint		iIndex = 0;

	/* 이 메시에 영향을 주는 ㅃㅕ의 갯수만큼 순회하자. */
	for (auto& iBoneIndex : m_BoneIndices)
	{
		XMStoreFloat4x4(&pBoneMatrices[iIndex],
			XMLoadFloat4x4(&m_OffsetMatrices[iIndex]) * Bones[iBoneIndex]->Get_CombinedTransformationMatrix() * PivotMatrix);

		++iIndex;
	}
}

HRESULT CMesh::DeserializeNonAnim(const char* pSaveFilePath)
{
#pragma region VERTEX_BUFFER
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//for (_uint i = 0; i < m_iNumVertices; i++)
	//{
	//	pVertices[i].Deserialize(pVertices[i], pSaveFilePath);
	//}
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(g_Pointer);
	
	is.read((char*)pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	/*for (_uint i = 0; i < m_iNumIndices; i++)
	{
		is.read((char*)&pIndices[i], sizeof(_ulong));
	}*/
	is.read((char*)pIndices, sizeof(_ulong) * m_iNumIndices);

	g_Pointer = is.tellg();

	m_iIndexStride = sizeof(_ulong);


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

#pragma region COOK
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	PxVec3* c = new PxVec3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		c[i] = { pVertices[i].vPosition.x,pVertices[i].vPosition.y,pVertices[i].vPosition.z };
	}

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

	Safe_Delete_Array(c);
	Safe_Delete_Array(d);

#pragma endregion

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::DeserializeAnim(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(CFile::Get_Instance()->GetPointer());

	is.read((char*)&m_iNumBones, sizeof(_uint));

	_uint matSize;
	is.read((char*)&matSize, sizeof(_uint));
	for (_uint i = 0; i < matSize; i++)
	{
		_float4x4 mat;
		is.read((char*)&mat, sizeof(_float4x4));
		m_OffsetMatrices.push_back(mat);
	}

	_uint boneIndices;
	is.read((char*)&boneIndices, sizeof(_uint));
	for (_uint i = 0; i < boneIndices; i++)
	{
		_uint index;
		is.read((char*)&index, sizeof(_uint));
		m_BoneIndices.push_back(index);
	}

#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	/*for (_uint i = 0; i < m_iNumVertices; i++)
		pVertices[i].Deserialize(pVertices[i],pSaveFilePath);*/

	is.read((char*)pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);


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

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	streampos p = is.tellg();
	/*for (_uint i = 0; i < m_iNumIndices; i++)
		is.read((char*)&pIndices[i], sizeof(_ulong));*/
	is.read((char*)pIndices, sizeof(_ulong) * m_iNumIndices);
	g_Pointer = is.tellg();

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
		return S_OK;
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix,const char* pSaveFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	ofstream os(pSaveFilePath, ios::binary | ios::app);
	strcpy_s(m_szName, pAIMesh->mName.data);
	_uint size = (_uint)strlen(m_szName);
	os.write((char*)&size, sizeof(_uint));
	os.write((char*)m_szName, sizeof(char) * size);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	os.write((char*)&m_iMaterialIndex, sizeof(_uint));
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	os.write((char*)&m_iNumVertices, sizeof(_uint));
	m_iStride = sizeof(VTXMESH);
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	os.write((char*)&m_iNumIndices, sizeof(_uint));
	m_iIndexStride = sizeof(_ulong); 
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	os.close();
 	HRESULT		hr = eModelType == CModel::TYPE_NONANIM ?
		Ready_Buffer_For_NonAnim(pAIMesh, PivotMatrix,pSaveFilePath) : Ready_Buffer_For_Anim(pAIMesh, pModel, pSaveFilePath);
	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Initialize_Deserialize(CModel::TYPE eModelType,const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(CFile::Get_Instance()->GetPointer());

	_uint size;
	is.read((char*)&size, sizeof(_uint));
	is.read((char*)m_szName, sizeof(char) * size);
	is.read((char*)&m_iMaterialIndex, sizeof(_uint));
	m_iNumVertexBuffers = 1;
	is.read((char*)&m_iNumVertices, sizeof(_uint));
	m_iStride = sizeof(VTXMESH);
	is.read((char*)&m_iNumIndices, sizeof(_uint));
	m_iIndexStride = sizeof(_ulong);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CFile::Get_Instance()->SetPointer(is.tellg());

	HRESULT		hr = eModelType == CModel::TYPE_NONANIM ?
		DeserializeNonAnim(pSaveFilePath) : DeserializeAnim(pSaveFilePath);
	if (FAILED(hr))
		return E_FAIL;
	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_Buffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix,const char* pSaveFilePath)
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	ofstream os(pSaveFilePath, ios::binary | ios::app);

	m_MinMaxPoint.first = pVertices[0].vPosition;
	m_MinMaxPoint.second = pVertices[0].vPosition;

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix)));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix)));

		/*pVertices[i].Serialize();*/
		/*os.write((char*)&pVertices[i].vPosition, sizeof(_float3));
		os.write((char*)&pVertices[i].vNormal, sizeof(_float3));
		os.write((char*)&pVertices[i].vTexcoord, sizeof(_float2));
		os.write((char*)&pVertices[i].vTangent, sizeof(_float3));*/

		m_MinMaxPoint.first.x = min(m_MinMaxPoint.first.x, pVertices[i].vPosition.x);
		m_MinMaxPoint.first.y = min(m_MinMaxPoint.first.y, pVertices[i].vPosition.y);
		m_MinMaxPoint.first.z = min(m_MinMaxPoint.first.z, pVertices[i].vPosition.z);

		m_MinMaxPoint.second.x = max(m_MinMaxPoint.second.x, pVertices[i].vPosition.x);
		m_MinMaxPoint.second.y = max(m_MinMaxPoint.second.y, pVertices[i].vPosition.y);
		m_MinMaxPoint.second.z = max(m_MinMaxPoint.second.z, pVertices[i].vPosition.z);
	}
	os.write((char*)pVertices, sizeof(VTXMESH) * m_iNumVertices);

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iIndex = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[0];
		//os.write((char*)&pIndices[iIndex++], sizeof(_ulong));
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[1];
		//os.write((char*)&pIndices[iIndex++], sizeof(_ulong));
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[2];
		//os.write((char*)&pIndices[iIndex++], sizeof(_ulong));
	}
	os.write((char*)pIndices, sizeof(_ulong) * m_iNumIndices);

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
	os.close();
	return S_OK;
}

HRESULT CMesh::Ready_Buffer_For_Anim(const aiMesh* pAIMesh, CModel* pModel, const char* pSaveFilePath)
{

#pragma region VERTEXBUFFER
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;


	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* 모델 = 메시의 집합.  */
	/* 메시가 사용하는 뼈의 갯수. */
	m_iNumBones = pAIMesh->mNumBones;

	/* 이 메시에게 영향을 주는 뼈들을 순회한다. */
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		/* 각 뼈정보에 접근한다. */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4		OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		/* pAIBone->mNumWeights : 특정 뼈는 몇개의 정점에게 영향을 주는가? */
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				/* pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices */
				/* 이 정점이 사용하고 있는 뼈(모델전체의뼈(x), 이 메시에게영향을 주는 뼈(o))의 인덱스다. */
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = (uint32_t)i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = (uint32_t)i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = (uint32_t)i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = (uint32_t)i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}


	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_float4x4		OffsetMatrix;

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));
	}

	ofstream os(pSaveFilePath, ios::binary | ios::app);

	os.write((char*)&m_iNumBones, sizeof(_uint));

	_uint offsetMatricesSize = (_uint)m_OffsetMatrices.size();
	os.write((char*)&offsetMatricesSize, sizeof(_uint));
	for (int i = 0; i<m_OffsetMatrices.size(); i++)
		os.write((char*)&m_OffsetMatrices[i], sizeof(_float4x4));

	_uint boneIndicesSize = (_uint)m_BoneIndices.size();
	os.write((char*)&boneIndicesSize, sizeof(_uint));
	for (int i = 0; i<m_BoneIndices.size(); i++)
		os.write((char*)&m_BoneIndices[i], sizeof(_uint));

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pVertices;


	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//for (_uint i = 0; i < m_iNumVertices; i++)
	//	pVertices[i].Serialize(pSaveFilePath);
	os.write((char*)pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iIndex = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[0];
		//os2.write((char*)&pIndices[iIndex++], sizeof(_ulong));
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[1];
		//os2.write((char*)&pIndices[iIndex++], sizeof(_ulong));
		pIndices[iIndex++] = pAIMesh->mFaces[i].mIndices[2];
		//os2.write((char*)&pIndices[iIndex++], sizeof(_ulong));
	}

	streampos p = os.tellp();
	os.write((char*)pIndices, sizeof(_ulong) * m_iNumIndices);
	streampos p2 = os.tellp();
	os.close();

	ZeroMemory(&m_InitializeData, sizeof m_InitializeData);
	m_InitializeData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix, const char* pSaveFilePath)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix, pSaveFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMesh");
	}

	return pInstance;
}

CMesh* CMesh::Deserialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const char* pSaveFilePath)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	pInstance->Initialize_Deserialize(eModelType, pSaveFilePath);

	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*	pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMesh");
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
