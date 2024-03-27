#pragma once

#include "Base.h"

/* �� ������ ǥ���ϱ����� ��ü. */
/* ������ ǥ���ϱ����� ����ϴ� ������ �ð��� ���� ��������� ���� �� �������ش�. */

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
	_double			m_Duration = { 0.0 }; /* �� �ִϸ��̼��� ����ϴµ� �ɸ��� �� �ð�. */
	_double			m_TickPerSecond = { 0.0 }; /* �ʴ� ����ؾ��ϴ� �ӵ�. */
	_double			m_TimeAcc = { 0.0 };

private: /* ���� */ /* CChannel : �� ���� �� �ִϸ��̼��� �����ϱ����� ��ü �ð� �ȿ��� ����ȭ�� �ð����� �� ���� ǥ���ؾ��� ��������� ������. */
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