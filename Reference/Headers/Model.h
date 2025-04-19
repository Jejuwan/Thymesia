#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	_uint Get_NumMaterials() const {
		return m_iNumMaterials;
	}
	vector<PxTriangleMesh*>& Get_TriangleMeshes() {
		return m_pTriangleMeshes;
	}
	void Set_BoundingBoxMinMaxPoint(const char* pSaveFilePath);
	pair<_float3,_float3> Get_BoundingBoxMinMaxPoint() {
		return m_BoundingBoxMinMaxPoint;
	}
	_uint Get_BoneIndex(const char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, _fmatrix PivotMatrix, const char* pModelFilePath, const char* pSaveFilePath);
	virtual HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const char* pContantName, _uint iMeshIndex);
	void Play_Animation(_float fTimeDelta);
	void Interpolate_Animation(_float fTimeDelta);
	void SetUp_Animation(_uint iIndex, _bool bLoop);
	void Set_Animation_Speed(_float fSpeed);
	_float Get_Animation_Speed();

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;
private:
	TYPE					m_eType = { TYPE_END };
	_float4x4				m_PivotMatrix;
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;
private:
	vector<class CBone*>			m_Bones;
private:
	_float4							m_RootPos = { 0,0,0,1 };
	_bool							m_bInterpolating = { false };
public:
	typedef vector<class CBone*>	BONES;

private:
	_uint							m_iNextAnimIndex = 0;
	_uint							m_iCurrentAnimIndex = 0;
	_uint							m_iNumAnimations = 0;
	_float							m_fInterpolateTime = 0.2f;
	vector<class CAnimation*>		m_Animations;
	_float4x4						m_BoneMatrices[256];
private:
	_uint					m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>	m_Materials;
	vector<PxTriangleMesh*> m_pTriangleMeshes;
	pair<_float3, _float3> m_BoundingBoxMinMaxPoint;
private:
	HRESULT Deserialize_Init(const char* pSaveFilePath);

	HRESULT Ready_Meshes(_fmatrix PivotMatrix, const char* pSaveFilePath);
	HRESULT Deserialize_Meshes(const char* pSaveFilePath);

	HRESULT Ready_Materials(const char* pModelFilePath, const char* pSaveFilePath);
	HRESULT Deserialize_Materials(const char* pSaveFilePath);


	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Deserialize_Bones(const char* pSaveFilePath);

public:
	HRESULT Ready_Animations(const char* pSaveFilePath);
	HRESULT Deserialize_Animations(const char* pSaveFilePath);
	HRESULT Serialize_Animations(const char* pSaveFilePath);
	HRESULT Serialize_Animations_Hard(const char* pSaveFilePath);

	HRESULT Deserialize_MinMaxPoint(const char* pSaveFilePath);

	CAnimation* Get_Animation() { return m_Animations[0]; }
	void Add_Animation(CAnimation* pAnim) { m_Animations.push_back(pAnim); }
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, const char* pSaveFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel* Deserialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pSaveFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	_float4 GetRootPos() { return m_RootPos; }
	_uint Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }
	_uint Get_NextAnimIndex() { return m_iNextAnimIndex; }
	CAnimation* Get_CurrentAnim() { return m_Animations[m_iCurrentAnimIndex]; }
	CAnimation* Get_Anim(_int idx) { return m_Animations[idx]; }
	void Set_NextAnim(_uint idx) { m_iNextAnimIndex = idx; }
	_float4x4 GetBoneMatrice(_uint idx) { return m_BoneMatrices[idx]; }
	vector<class CBone*>& GetBones() { return m_Bones; }
	_float4x4 GetPivotMatrix() { return m_PivotMatrix; }
	void SetPivotMatrix(_matrix mat) { XMStoreFloat4x4(&m_PivotMatrix, mat); }
	void SetInterpolateTime(_float f) { m_fInterpolateTime = f; }
};

END