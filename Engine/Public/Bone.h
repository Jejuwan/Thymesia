#pragma once

/* 뼈의 정보를 표현한다.*/
#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTrasnformationMatrix);
	}

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
	HRESULT Serialize(const char* pSaveFilePath);
	HRESULT Initialize_Deserialize(const char* pSaveFilePath);
	void Update_CombinedTransformationMatrix(const CModel::BONES& Bones);

private:
	char			m_szName[MAX_PATH] = "";
	_float4x4		m_TransformationMatrix; /* 부모기준으로 표현된 나만의 상태행렬 */
	_float4x4		m_CombinedTrasnformationMatrix; /* 부모상태를 포함한 이 뼈를 그리기위한 최종 행렬. */
	_int			m_iParentIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	static CBone* Deserialize(const char* pSaveFilePath);
	CBone* Clone();
	virtual void Free();
};

END