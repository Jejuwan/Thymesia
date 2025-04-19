#pragma once

#include "Model.h"

/* 1. Ư�� �ִϸ��̼� ������ ó������ ������ �����ϱ����� ������ ������ �ִ´�. */
/* 2. �����ϱ����� ���� : */
/* 2-1. �� �ִϸ��̼��� ǥ���ϱ����� ������ ����. ������ ����. �� : ä�� */
/* 2-2. �ִϸ��̼��� �ӵ�(TickPerSecond), �ִϸ��̼��� ��ü �����ϴµ� �ɸ��� �ð�(Duration) */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel, const char* pSaveFilePath);
	HRESULT Initialize_Deserialize(const char* pSaveFilePath);
	HRESULT Update_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones);
	void Interpolate_TransformationMatrices(_float fTimeDelta, class CAnimation* _pAnim, const CModel::BONES& Bones,_float InterpolateTime);
	void Serialize(const aiAnimation* pAIAnimation, CModel* pModel, const char* pSaveFilePath);

private:
	_float				m_fDuration = { 0.0f };
	_float				m_fTickPerSecond = { 0.0f };
	_float				m_fTrackPosition = { 0.0f };
	_float				m_fSpeed = { 1.0f };
private:
	/* �ִϸ��̼��� �����ϱ����� ���� ���� .*/
	_uint						m_iNumChannels = { 0 };
	/* �ִϸ��̼��� �����ϱ����� ���� .*/
	vector<class CChannel*>		m_Channels;
	//�� ä�� �� Ű������
	vector<_uint>				m_CurrentKeyFrames;

private:
	_float4						m_RootPos = { 0,0,0,1 };
	_bool						m_bLoop = false;
	_bool						m_bEnd = false;
	_bool						m_bInterpolateEnd = false;
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel, const char* pSaveFilePath);
	static CAnimation* Deserialize(const char* pSaveFilePath);
	CAnimation* Clone();
	virtual void Free() override;
public:
	_float4						GetRootPos() { return m_RootPos; }
	_bool						GetLoop() { return m_bLoop; }
	void						SetLoop(_bool loop) { m_bLoop = loop; }
	_bool						GetEnd() { return m_bEnd; }
	void						SetEnd(_bool b) { m_bEnd = b; }
	void						SetEndForInterpolate()
	{
		m_bEnd = true;
		m_bLoop = false;
		m_fTrackPosition = 0.f;
	}
	void						ResetEnd() 
	{
		m_bEnd = false;
		m_bInterpolateEnd = false;
		m_fTrackPosition = 0.f;
		for (auto& keyFrame : m_CurrentKeyFrames)
			keyFrame = 0;
	}
	vector<class CChannel*>&	GetChannels() { return m_Channels; }
	_bool						GetInterpolateEnd() { return m_bInterpolateEnd; }
	_float						GetDuration() { return m_fDuration; }
	_float						GetTrackPosition() { return m_fTrackPosition; }
	void						Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	_float						Get_Speed() { return m_fSpeed; }
};

END