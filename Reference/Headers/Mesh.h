#pragma once

/* ���� �����ϴ� �ϳ��� ����. */
/* �� ������ ������ �ε������۷� �����Ǿ��ִ�. */

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
protected:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	PxTriangleMesh* Get_TriangleMesh() {
		return m_pTriangleMesh;
	}
	pair<_float3, _float3> Get_MinMaxPoint() {
		return m_MinMaxPoint;
	}
	void Get_BoneMatrices(_float4x4* pBoneMatrices, const CModel::BONES& Bones, _fmatrix PivotMatrix);
	HRESULT DeserializeNonAnim(const char* pSaveFilePath);
	HRESULT DeserializeAnim(const char* pSaveFilePath);
public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix,const char* pSaveFilePath);
	HRESULT Initialize_Deserialize(CModel::TYPE eModelType, const char* pSaveFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	char						m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	PxTriangleMesh* m_pTriangleMesh = { nullptr };
	pair<_float3, _float3> m_MinMaxPoint;

	_uint						m_iNumBones = { 0 };
	vector<_uint>				m_BoneIndices; /* ���� ������ �ִ� ��ü���� �ε��� */
	vector<_float4x4>			m_OffsetMatrices;
	wstring					m_SaveFilePath = TEXT("");
private:
	HRESULT Ready_Buffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix, const char* pSaveFilePath);
	HRESULT Ready_Buffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel, const char* pSaveFilePath);
public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, CModel* pModel, _fmatrix PivotMatrix,const char* pSaveFilePath);
	static CMesh* Deserialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const char* pSaveFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END