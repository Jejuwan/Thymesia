#include "Engine_Pch.h"
#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "File.h"



CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_pTriangleMeshes(rhs.m_pTriangleMeshes)
	,m_BoundingBoxMinMaxPoint(rhs.m_BoundingBoxMinMaxPoint)
{
	for (auto& pOriginalAnimation : rhs.m_Animations)
	{
		m_Animations.push_back(pOriginalAnimation->Clone());
	}

	for (auto& pOriginalBone : rhs.m_Bones)
	{
		m_Bones.push_back(pOriginalBone->Clone());
	}

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			Safe_AddRef(m_Materials[i].pMaterialTexture[j]);
		}
	}

	for (size_t i = 0; i < m_iNumMeshes; i++)
		Safe_AddRef(m_Meshes[i]);

}

void CModel::Set_BoundingBoxMinMaxPoint(const char* pSaveFilePath)
{
	m_BoundingBoxMinMaxPoint = m_Meshes[0]->Get_MinMaxPoint();
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		m_BoundingBoxMinMaxPoint.first.x = min(m_BoundingBoxMinMaxPoint.first.x, m_Meshes[i]->Get_MinMaxPoint().first.x);
		m_BoundingBoxMinMaxPoint.first.y = min(m_BoundingBoxMinMaxPoint.first.y, m_Meshes[i]->Get_MinMaxPoint().first.y);
		m_BoundingBoxMinMaxPoint.first.z = min(m_BoundingBoxMinMaxPoint.first.z, m_Meshes[i]->Get_MinMaxPoint().first.z);

		m_BoundingBoxMinMaxPoint.second.x = max(m_BoundingBoxMinMaxPoint.second.x, m_Meshes[i]->Get_MinMaxPoint().second.x);
		m_BoundingBoxMinMaxPoint.second.y = max(m_BoundingBoxMinMaxPoint.second.y, m_Meshes[i]->Get_MinMaxPoint().second.y);
		m_BoundingBoxMinMaxPoint.second.z = max(m_BoundingBoxMinMaxPoint.second.z, m_Meshes[i]->Get_MinMaxPoint().second.z);
	}
	ofstream os(pSaveFilePath, ios::binary | ios::app);
	streampos p = os.tellp();
	os.write((char*) &m_BoundingBoxMinMaxPoint, sizeof(pair<_float3, _float3>));
	os.close();
}

_uint CModel::Get_BoneIndex(const char* pBoneName)
{
	_uint		iNumBones = (_uint)m_Bones.size();
	size_t i = 0;
	for (; i < iNumBones; i++)
	{
		if (false == strcmp(m_Bones[i]->Get_Name(), pBoneName))
			break;
	}

	return (_uint)i;
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, _fmatrix PivotMatrix, const char* pModelFilePath,const char* pSaveFilePath)
{
	/* m_Importer라는 객체를 통해서 파일의 정보를 읽어들여 m_pAIScene객체에 모든 데이터를 저장한다. */
	m_eType = eModelType;

	ofstream os(pSaveFilePath, ios::binary);
	if (m_eType == TYPE_NONANIM)
	{
		_uint type = 1;
		os.write((char*)&type, sizeof(_uint));
	}
	else if (m_eType == TYPE_ANIM)
	{
		_uint type = 2;
		os.write((char*)&type, sizeof(_uint));
	}

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	os.write((char*)&m_PivotMatrix, sizeof _float4x4);

	streampos p = os.tellp();
	os.close();
	_uint		iFlag = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;

	if (TYPE_NONANIM == m_eType)
		iFlag |= aiProcess_PreTransformVertices;

	// aiProcess_GlobalScale
	/* aiProcess_PreTransformVertices : 모델안에 포함되어있는 뼈대의 정보를 메시에게 적용해준다. */

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* m_pAIScene이라는 객체가 들고 있는 정보를 정리하여 내가 사용하기 좋게 만든다. */

	/* 3. 뼈를 로드하낟. (aiNode, aiBone, aiNodeAnim) */
	/* aiNode : 뼈의 상속관계를 표현한다. (자식과 부모의 정보, 행렬)*/
	/* aiBone : 메시기준으로 묶여있는 뼈의 정보. 이 뼈는 어떤 정점에게 영햐응ㄹ 주는가? 몇퍼나 영향르 주는가? */
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;
	ofstream os2(pSaveFilePath, ios::binary | ios::app);
	_uint boneSize = (_uint)m_Bones.size();
	os2.write((char*)&boneSize, sizeof(_uint));
	os2.close();
	for (auto& bone : m_Bones)
		bone->Serialize(pSaveFilePath);

	/* 1. 메시의 정보를 생성한다.
		메시 : 삼각형 집합. (정점버퍼, 인덱스버퍼를 통해서 만든다.)
		모델 : 메시의 집합. 추후 진행에서 메시의 파츠 교체가 용이하도록ㅎ하기위해서 .*/
	if (FAILED(Ready_Meshes(PivotMatrix, pSaveFilePath)))
		return E_FAIL;

	/* 2. 메시의 머테리얼을 로드하낟.(텍스쳐를 로드한다.) */
	if (FAILED(Ready_Materials(pModelFilePath, pSaveFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations(pSaveFilePath)))
		return E_FAIL;

	Set_BoundingBoxMinMaxPoint(pSaveFilePath);

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	for(int i=0;i<m_pTriangleMeshes.size();i++)
	{
		PxTriangleMeshGeometry hfGeom(m_pTriangleMeshes[i]);

		PxRigidStatic* hfActor = CGameInstance::Get_Instance()->GetPhysics()->createRigidStatic(PxTransform(PxIdentity));
		PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *CGameInstance::Get_Instance()->GetMaterial());
	}
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (nullptr != m_Meshes[iMeshIndex])
		m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

CModel* CModel::Deserialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pSaveFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	pInstance->Deserialize_Init(pSaveFilePath);
	pInstance->Deserialize_Bones(pSaveFilePath);
	pInstance->Deserialize_Meshes(pSaveFilePath);
	pInstance->Deserialize_Materials(pSaveFilePath);
	pInstance->Deserialize_MinMaxPoint(pSaveFilePath);
	if(eModelType==TYPE_ANIM)
		pInstance->Deserialize_Animations(pSaveFilePath);

	ifstream is(pSaveFilePath, ios::binary);
	is.close();
	g_Pointer = 0;
	return pInstance;
}

HRESULT CModel::Bind_ShaderResourceView(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	CTexture* tex = m_Materials[iMaterialIndex].pMaterialTexture[eTextureType];
	if(nullptr != tex)
		tex->Bind_ShaderResourceView(pShader, pConstantName, 0);
	else
	{
		int a = 5;
	}

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const char* pContantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Get_BoneMatrices(m_BoneMatrices, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));

	return pShader->Bind_Matrices(pContantName, m_BoneMatrices, 256);
}

void CModel::Play_Animation(_float fTimeDelta)
{
	
		/* 현재 애니메이션이 사용하는 뼈들의 TransformationMatrix 를 갱신한다. */
		/* 1. 현재 애니메이션이 사용하는 뼈?! */
		/* 2. 뼈들의 상태는 어떤 값으로 갱신을해야하는지?! */
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones);

		m_RootPos = m_Animations[m_iCurrentAnimIndex]->GetRootPos();

		/* 특정 뼈들의 TransfokrmationMatrix가 변경되었기때문에. 부모로 부터 누적하여 만드는 CombinedTransfromationMatrix의 상태를 갱신해준다. */
		/* m_Bones에서 하나씩 꺼내서 CombinedTransformation을 갱신한다. */
		for (auto& pBone : m_Bones)
		{
			pBone->Update_CombinedTransformationMatrix(m_Bones);
		}
	
	//else
	//{
	//	Interpolate_Animation(m_iNextAnimIndex, fTimeDelta);
	//}
}

void CModel::Interpolate_Animation(_float fTimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Interpolate_TransformationMatrices(fTimeDelta,m_Animations[m_iNextAnimIndex],m_Bones,m_fInterpolateTime);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones);
	}
}

void CModel::SetUp_Animation(_uint iIndex, _bool bLoop)
{
	if (iIndex >= m_iNumAnimations)
		return;

	m_iCurrentAnimIndex = iIndex;
	m_Animations[m_iCurrentAnimIndex]->SetLoop(bLoop);

}

void CModel::Set_Animation_Speed(_float fSpeed)
{
	for (auto& anim : m_Animations)
	{
		anim->Set_Speed(fSpeed);
	}
}

_float CModel::Get_Animation_Speed()
{
	 return m_Animations[0]->Get_Speed(); 
}

HRESULT CModel::Deserialize_Init(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(g_Pointer);
 	_uint type = 0;
	is.read((char*)&type, sizeof(_uint));
	if (type == 1)
		m_eType = TYPE_NONANIM;
	else if (type == 2)
		m_eType = TYPE_ANIM;

	is.read((char*)&m_PivotMatrix, sizeof(_float4x4));
	g_Pointer= (int)is.tellg();
	return S_OK;
}

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix,const char* pSaveFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	ofstream os(pSaveFilePath, ios::binary | ios::app);
	m_iNumMeshes = m_pAIScene->mNumMeshes;
	os.write((char*)&m_iNumMeshes, sizeof(_uint));
	os.close();

 	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext,m_eType, m_pAIScene->mMeshes[i],this, PivotMatrix, pSaveFilePath);
		if (nullptr == pMesh)
			return E_FAIL;
		if (nullptr != pMesh->Get_TriangleMesh())
			m_pTriangleMeshes.push_back(pMesh->Get_TriangleMesh());
		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Deserialize_Meshes(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	int p = g_Pointer;
	is.seekg(CFile::Get_Instance()->GetPointer());
	is.read((char*)&m_iNumMeshes, sizeof(_uint));
	CFile::Get_Instance()->SetPointer(is.tellg());

     	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = pMesh->Deserialize(m_pDevice, m_pContext, m_eType, pSaveFilePath);
		if (nullptr == pMesh)
			return E_FAIL;
		if (nullptr != pMesh->Get_TriangleMesh())
			m_pTriangleMeshes.push_back(pMesh->Get_TriangleMesh());
		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath,const char* pSaveFilePath)
{
	ofstream os(pSaveFilePath, ios::binary | ios::app);
	m_iNumMaterials = m_pAIScene->mNumMaterials;
	os.write((char*)&m_iNumMaterials, sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof ModelMaterial);

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
			{
				_int wrong = -1;
				os.write((char*)&wrong, sizeof(_int));
				continue;
			}

			const char* cstr = strPath.C_Str();
			//if (cstr[0] == 'M' && cstr[1] == 'I')
			//{
			//	m_iNumMaterials = m_Materials.size();
			//	return S_OK;
			//}

			char		szDrive[MAX_PATH] = "";
 			char		szDirectory[MAX_PATH] = "";
			char		szFileName[MAX_PATH] = "";
			char		szExt[MAX_PATH] = "";
			char		szFullPath[MAX_PATH] = "";
			_tchar		szFinalPath[MAX_PATH] = L"";

			/* j번째(Diffuse, Specular, Normals ... ) 타입의 텍스쳐가 존재한다. */
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szExt, ".dds");
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath),
				szFinalPath, MAX_PATH);

			if (!filesystem::exists(szFinalPath))
			{
				_int wrong = -1;
				os.write((char*)&wrong, sizeof(_int));
				continue;
			}
			_int size = (int)wcslen(szFinalPath);
			os.write((char*)&size, sizeof(_int));
			os.write((char*)&szFinalPath, sizeof(_tchar) * size);
			os.write((char*)&j, sizeof(size_t));

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath);
 			if (nullptr == ModelMaterial.pMaterialTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}
	os.close();
	return S_OK;
}

HRESULT CModel::Deserialize_Materials(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(CFile::Get_Instance()->GetPointer());
	is.read((char*)&m_iNumMaterials, sizeof(_uint));
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof ModelMaterial);

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			_tchar		szFinalPath[MAX_PATH] = L"";

			_int size;
			is.read((char*)&size, sizeof(_int));

			if (size == -1)
				continue;

			is.read((char*)&szFinalPath, sizeof(_tchar) * size);
			is.read((char*)&j, sizeof(size_t));

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath);
			if (nullptr == ModelMaterial.pMaterialTexture[j])
				return E_FAIL;
		}
		m_Materials.push_back(ModelMaterial);
	}
	CFile::Get_Instance()->SetPointer(is.tellg());
	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	for (size_t i = 0; i < pAINode->mNumChildren; ++i)
	{
		if (FAILED(Ready_Bones(pAINode->mChildren[i], Get_BoneIndex(pBone->Get_Name()))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Deserialize_Bones(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(g_Pointer);
	_uint boneSize = (_uint)m_Bones.size();
	is.read((char*)&boneSize, sizeof(_uint));
	g_Pointer = (int)is.tellg();

	for(_uint i=0;i<boneSize;i++)
	{
		CBone* pBone = CBone::Deserialize(pSaveFilePath);

		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(const char* pSaveFilePath)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this, pSaveFilePath);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Deserialize_Animations(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	streampos p = is.tellg();
	is.seekg(CFile::Get_Instance()->GetPointer());
	is.read((char*)&m_iNumAnimations, sizeof(_uint));
	CFile::Get_Instance()->SetPointer(is.tellg());
	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Deserialize(pSaveFilePath);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HRESULT CModel::Serialize_Animations(const char* pSaveFilePath)
{
	_uint iNumAnimations = m_pAIScene->mNumAnimations;
	ofstream os(pSaveFilePath, ios::binary | ios::app);
	_uint iModified = iNumAnimations - 1;
	os.write((char*)&iModified, sizeof(_uint));
	os.close();

	//추출시 애니메이션의 첫번째 파일에 모든 애니메이션이 합쳐져있는 버그있어서 스킵
	for(_uint i=1;i< iNumAnimations;i++)
	m_Animations[i]->Serialize(m_pAIScene->mAnimations[i], this, pSaveFilePath);
	
	return S_OK;
}

HRESULT CModel::Serialize_Animations_Hard(const char* pSaveFilePath)
{
	m_Animations[0]->Serialize(m_pAIScene->mAnimations[0], this, pSaveFilePath);

	return S_OK;
}

HRESULT CModel::Deserialize_MinMaxPoint(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(g_Pointer);
	is.read((char*)&m_BoundingBoxMinMaxPoint, sizeof(pair<_float3, _float3>));
	g_Pointer = (int)is.tellg();
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, const char* pSaveFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, PivotMatrix, pModelFilePath, pSaveFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CModel");
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CModel");
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();


	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			Safe_Release(m_Materials[i].pMaterialTexture[j]);
		}
	}

	for (size_t i = 0; i < m_iNumMeshes; i++)
		Safe_Release(m_Meshes[i]);
}
