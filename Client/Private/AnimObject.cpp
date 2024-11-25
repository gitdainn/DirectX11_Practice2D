#include "stdafx.h"
#include "AnimObject.h"

CAnimObject::CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{
}

CAnimObject::CAnimObject(const CAnimObject& rhs)
	: CSpriteObject(rhs)
	, m_iUVTextureIndex(rhs.m_iUVTextureIndex), m_iUVTexNumX(rhs.m_iUVTexNumX), m_iUVTexNumY(rhs.m_iUVTexNumY)
	, m_iAnimTypeNum(rhs.m_iAnimTypeNum), m_iAnimType(rhs.m_iAnimType), m_AnimAcc(rhs.m_AnimAcc)
	, m_bIsLoop(rhs.m_bIsLoop)
{
	if (nullptr != rhs.m_pAnimInfo)
	{
		if (1 == m_iAnimTypeNum)
		{
			m_pAnimInfo = new ANIM_INFO;
			m_pAnimInfo->fAnimTime = rhs.m_pAnimInfo->fAnimTime;
			m_pAnimInfo->iStartIndex = rhs.m_pAnimInfo->iStartIndex;
			m_pAnimInfo->iEndIndex = rhs.m_pAnimInfo->iEndIndex;
			m_pAnimInfo->fDelayTimeMap = rhs.m_pAnimInfo->fDelayTimeMap;
		}
		else
		{
			m_pAnimInfo = new ANIM_INFO[m_iAnimTypeNum]{};
			for (_uint i = 0; i < m_iAnimTypeNum; ++i)
			{
				m_pAnimInfo[i].fAnimTime = rhs.m_pAnimInfo[i].fAnimTime;
				m_pAnimInfo[i].iStartIndex = rhs.m_pAnimInfo[i].iStartIndex;
				m_pAnimInfo[i].iEndIndex = rhs.m_pAnimInfo[i].iEndIndex;
				m_pAnimInfo[i].fDelayTimeMap = rhs.m_pAnimInfo[i].fDelayTimeMap;
			}
		}
	}
}

void CAnimObject::Play_Animation(_double TimeDelta, _uint& iSpriteIndex, const _uint iAnimType)
{
	m_bIsEndSprite = false;

	ANIM_INFO* pAnimInfo = m_pAnimInfo + iAnimType;
	if (nullptr == pAnimInfo)
		return;
	_float fPerAnimTime = pAnimInfo->fAnimTime / fabs((_float)pAnimInfo->iEndIndex - (_float)pAnimInfo->iStartIndex);

	const _uint iCurrentSpriteIndex = m_bIsAnimUV ? m_iUVTextureIndex : m_iTextureIndex;
	unordered_map<_uint, _float>::iterator iter = pAnimInfo->fDelayTimeMap.find(iCurrentSpriteIndex);
	_float fDelayTime = { 0.f };
	if (iter != pAnimInfo->fDelayTimeMap.end())
		fDelayTime = iter->second;

	m_AnimAcc += (_float)TimeDelta;
	if (fPerAnimTime + fDelayTime <= m_AnimAcc)
	{
		m_AnimAcc = 0.f;
		++iSpriteIndex;

		if (pAnimInfo->iEndIndex < iSpriteIndex)
		{
			if (m_bIsLoop)
				iSpriteIndex = pAnimInfo->iStartIndex;
			else
				End_Animation(iSpriteIndex);
		}
	}
}

void CAnimObject::End_Animation(_uint& iSpriteIndex)
{
	m_bIsEndSprite = true;
}

HRESULT CAnimObject::SetUp_Shader_UVAnim()
{
	if (0 == m_iUVTexNumX)
	{
		MSG_BOX("CAnimObject - SetUp_Shader_UVAnim - m_iUVTexNum이 0입니다.");
		return E_FAIL;
	}
	_uint iUVIndexY = m_iUVTextureIndex / m_iUVTexNumX;
	/** @note - _uint가 있으면 int로 담기 전 계산 과정에서 이미 모두 int로 변환 후 계산해야함. (음수가 되면 엄청 쓰레기값 들어감) */
	_uint iUVIndexX = max(0, (int)m_iUVTextureIndex - (int)(m_iUVTexNumX * iUVIndexY) - 1);

	// 0일 경우 -1을 하면 _uint라 이상한 값 나오기 때문에 체크 후 1 감소 (1감소해야 위치 맞음)
	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexX", &iUVIndexX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexY", &iUVIndexY, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumX", &m_iUVTexNumX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumY", &m_iUVTexNumY, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

void CAnimObject::Free()
{
	__super::Free();

	if (nullptr != m_pAnimInfo)
		m_pAnimInfo->fDelayTimeMap.clear();

	if (1 < m_iAnimTypeNum)
		Safe_Delete_Array(m_pAnimInfo);
	else
		Safe_Delete(m_pAnimInfo);
}
