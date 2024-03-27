#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_TimeAcc(rhs.m_TimeAcc)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)
	, m_isLoop(rhs.m_isLoop)
	, m_isFinished(rhs.m_isFinished)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	/* 이 애니메이션이 사용하고 있는 뼈의 갯수만큼 커런트키프레임을 표현할 데이터를 미리 추가확보한다. */
	m_iCurrentKeyFrames.resize(m_iNumChannels);

	return S_OK;
}

void CAnimation::Update(vector<CBone*>& Bones, _double TimeDelta)
{
	if (m_TimeAcc < m_Duration)
		m_isFinished = false;

	/* m_TimeAcc : 현재 애니메이션이 재생된 시간. */
	m_TimeAcc += m_TickPerSecond * TimeDelta;

	if (m_TimeAcc >= m_Duration)
	{
		m_isFinished = true;

		if (true == m_isLoop)
		{
			m_TimeAcc = 0.0;			
		}
	}

	/* 이 애님을 표현하는데 필요한 모든 뼈대들의 행렬을 키프레임정보로 만들어낸다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update(Bones, &m_iCurrentKeyFrames[i], m_TimeAcc);
	}



}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel* pModel)
{
	CAnimation *	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

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

