#include "Engine_Pch.h"
#include "..\Public\Animation.h"
#include "Channel.h"
#include "File.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_Channels(rhs.m_Channels)
	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, CModel* pModel,const char* pSaveFilePath)
{
	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇개의 뼈를 구동하여 표현하는가 .*/
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrames.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel, pSaveFilePath);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

HRESULT CAnimation::Initialize_Deserialize(const char* pSaveFilePath)
{
	ifstream is(pSaveFilePath, ios::binary);
	is.seekg(CFile::Get_Instance()->GetPointer());

	is.read((char*)&m_fDuration, sizeof(_float));
	is.read((char*)&m_fTickPerSecond, sizeof(_float));
	is.read((char*)&m_iNumChannels, sizeof(_uint));
	g_Pointer = (int)is.tellg();
	m_CurrentKeyFrames.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Deserialize(pSaveFilePath);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

HRESULT CAnimation::Update_TransformationMatrices(_float fTimeDelta, const CModel::BONES& Bones)
{
	/* 현재 애니메이션이 재생해야할 위치를 구한다. */
	if(!m_bEnd)
		m_fTrackPosition += m_fTickPerSecond * m_fSpeed * fTimeDelta;

	if (m_fTrackPosition >= m_fDuration)
	{
		if (m_bLoop)
			m_fTrackPosition = 0.f;
		else
		{
			m_bEnd = true;
			m_fTrackPosition = 0.f; 
		}
	}

	if (m_bEnd)
		return S_OK;

	_uint		iIndex = 0;

	/* 이 애니메이션에서 사용하는 모든 뼈들을 순회하면서 시간에 맞는 상태를 가질 수 있도록 한다. */
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrices(&m_CurrentKeyFrames[iIndex++], m_fTrackPosition, Bones);
		if (pChannel->GetIndex() == 2)
		{
			m_RootPos = pChannel->GetRootPos();
		}
	}

	return S_OK;
}

void CAnimation::Interpolate_TransformationMatrices(_float fTimeDelta, CAnimation* _pAnim, const CModel::BONES& Bones,_float InterpolateTime)
{
	m_fTrackPosition += fTimeDelta;
	if (m_fTrackPosition >= InterpolateTime)
	{
		m_bInterpolateEnd = true;
	}
	_uint		iIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Interpolate_TransformationMatrices(_pAnim->GetChannels()[iIndex], &m_CurrentKeyFrames[iIndex], m_fTrackPosition, Bones, InterpolateTime);
		//if (pChannel->GetIndex() == 2)
		//{
		//	m_RootPos = pChannel->GetRootPos();
		//}
		iIndex++;
	}
}

void CAnimation::Serialize(const aiAnimation* pAIAnimation, CModel* pModel, const char* pSaveFilePath)
{
	ofstream os(pSaveFilePath, ios::binary | ios::app);
	os.write((char*)&m_fDuration, sizeof(_float));
	os.write((char*)&m_fTickPerSecond, sizeof(_float));
	os.write((char*)&m_iNumChannels, sizeof(_uint));
	os.close();
	                                                                                               
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Serialize(pAIAnimation->mChannels[i], pModel, pSaveFilePath);
	}
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, class CModel* pModel, const char* pSaveFilePath)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel,pSaveFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CAnimation");
	}

	return pInstance;
}

CAnimation* CAnimation::Deserialize(const char* pSaveFilePath)
{
	CAnimation* pInstance = new CAnimation();

	pInstance->Initialize_Deserialize(pSaveFilePath);

	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
