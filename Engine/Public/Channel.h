#pragma once

#include "Base.h"

/* Ư�� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ��, �ϳ�. */
/* �ð��� ���� �� ���� ���µ��� ������. */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void Update(vector<class CBone*>& Bones, _uint* pCurrentKeyFrame, _double CurrentTime);

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iBoneIndex = { 0 };
public:
	static CChannel * Create(aiNodeAnim * pAIChannel, CModel * pModel);
	virtual void Free() override;
};

END