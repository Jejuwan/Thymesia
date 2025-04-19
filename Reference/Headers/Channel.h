#pragma once

/* 특정 애니메이션을 구동하기위한 뼈들 중 하나의 뼈.  */
#include "Model.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel, const char* pSaveFilePath);
	HRESULT Initialize_Deserialize(const char* pSaveFilePath);
	void Update_TransformationMatrices(_uint* pCurrentKeyFrameIndex, _float fTrackPosition, const CModel::BONES& Bones);
	void Interpolate_TransformationMatrices(class CChannel* pChannel, _uint* pCurrentKeyFrameIndex, _float fTrackPosition, const CModel::BONES& Bones,_float InterpolateTime);

	void Serialize(const aiNodeAnim* pAIChannel, CModel* pModel, const char* pSaveFilePath);
private:
	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;
	
	

	/* 채널이 생성될 때 이름이 같은 뼈의 인덱스를 들고 있게 하겠다. */
	_uint					m_iBoneIndex;
	_float4					m_RootPos = { 0,0,0,1 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, CModel* pModel, const char* pSaveFilePath);
	static CChannel* Deserialize(const char* pSaveFilePath);
	virtual void Free() override;
public:
	_uint GetIndex() { return m_iBoneIndex; }
	_float4 GetRootPos() { return m_RootPos; }
	vector<KEYFRAME>& GetKeyFrames() { return m_KeyFrames; }
};

END