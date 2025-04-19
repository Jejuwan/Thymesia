#include "Engine_Pch.h"
#include "..\Public\Bone.h"
#include "File.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode * pAINode, _int iParentIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTrasnformationMatrix, XMMatrixIdentity());

	m_iParentIndex = iParentIndex;	

	return S_OK;
}

HRESULT CBone::Serialize(const char* pSaveFilePath)
{
	ofstream os(pSaveFilePath, ios::binary | ios::app);
	_uint size = (_uint)strlen(m_szName);
	os.write((char*)&size, sizeof(_uint));
	os.write((char*)m_szName, sizeof(char) * size);
	os.write((char*)&m_TransformationMatrix, sizeof(_float4x4));
	os.write((char*)&m_iParentIndex, sizeof(_int));
	os.close();
	return S_OK;
}

HRESULT CBone::Initialize_Deserialize(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(g_Pointer);
	_uint size;
	is.read((char*)&size, sizeof(_uint));
	is.read((char*)m_szName, sizeof(char) * size);
	is.read((char*)&m_TransformationMatrix, sizeof(_float4x4));
	is.read((char*)&m_iParentIndex, sizeof(_int));
	CFile::Get_Instance()->SetPointer(is.tellg());
	return S_OK;
}



void CBone::Update_CombinedTransformationMatrix(const CModel::BONES& Bones)
{
	/* m_CombinedTransformationMatrix = m_TransformationMatrix * 부모`s m_CombinedTransformationMatrix */
	/* m_CombinedTransformationMatrix를 갱신한다. */

	if (0 > m_iParentIndex)
		m_CombinedTrasnformationMatrix = m_TransformationMatrix;
	else
		XMStoreFloat4x4(&m_CombinedTrasnformationMatrix, 
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTrasnformationMatrix));
}

CBone * CBone::Create(const aiNode * pAINode, _int iParentIndex)
{
	CBone*	pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CBone");
	}

	return pInstance;
}

CBone* CBone::Deserialize(const char* pSaveFilePath)
{
	CBone* pInstance = new CBone();

	pInstance->Initialize_Deserialize(pSaveFilePath);

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);	
}

void CBone::Free()
{
	
}
