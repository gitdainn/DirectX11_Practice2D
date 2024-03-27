#pragma once

#include "Base.h"

/* 한 동작을 표현하기위한 객체. */
/* 동작을 표현하기위해 사용하는 뼈들의 시간에 따른 상태행렬을 보관 및 갱신해준다. */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void Update(vector<class CBone*>& Bones, _double TimeDelta);

private:
	char			m_szName[MAX_PATH] = "";
	_double			m_Duration = { 0.0 }; /* 이 애니메이션을 재생하는데 걸리는 총 시간. */
	_double			m_TickPerSecond = { 0.0 }; /* 초당 재생해야하는 속도. */
	_double			m_TimeAcc = { 0.0 };

private: /* 뼈들 */ /* CChannel : 이 뼈가 이 애니메이션을 구동하기위한 전체 시간 안에서 세분화된 시간마다 이 뼈가 표현해야할 행렬정보를 가진다. */
	_uint								m_iNumChannels = { 0 };
	vector<class CChannel*>				m_Channels;

	vector<_uint>						m_iCurrentKeyFrames;

	_bool								m_isLoop = { true };
	_bool								m_isFinished = { false };

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone();
	virtual void Free() override;
};

END