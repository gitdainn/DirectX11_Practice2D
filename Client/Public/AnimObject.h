#pragma once
#include "SpriteObject.h"

class CAnimObject abstract : public CSpriteObject
{
protected:
	explicit CAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAnimObject(const CAnimObject& rhs);
	virtual ~CAnimObject() = default;

protected:
	struct ANIM_INFO
	{
		unordered_map<_uint, _float> fDelayTimeMap;
		_float fAnimTime = { 0.f };
		// @qurious - vector를 하려면 딜레이없는 애들까지 공간 다 만들어줘야하고, 공간을 아끼자니 몇번째 인덱스인지 .find()를 써야함 ㅁ가 더 좋을까?
		/** @qurious - map 타입 정의에는 const 쓰면 안됨 */
		_uint iStartIndex = { 0 };
		_uint iEndIndex = { 0 };
	};

public:
	void Change_AnimType(const _uint iAnimType)
	{
		if (m_iAnimTypeNum <= iAnimType)
			return;

		m_iAnimType = iAnimType;
		const ANIM_INFO* pAnim = m_pAnimInfo + m_iAnimType;
		m_iUVTextureIndex = pAnim->iStartIndex;
	}

protected:
	virtual void	Switch_SpriteDirection() override
	{
		__super::Switch_SpriteDirection();
		m_iShaderPassIndex = ((m_eSpriteDirection == SPRITE_DIRECTION::LEFT ? (_uint)VTXTEX_PASS::FlipUV_Anim : (_uint)VTXTEX_PASS::UV_Anim));
	}

protected: 
	virtual void Add_Animation() = 0;
	virtual void Play_Animation(_double TimeDelta, _uint& iSpriteIndex, const _uint iAnimType = 0);
	virtual void End_Animation(_uint& iSpriteIndex);

protected:
	virtual HRESULT SetUp_ShaderResources() = 0;

	/** UV 애니메이션을 진행하기 위해 필수적으로 넘겨야 하는 정보입니다. 꼭 포함해주세요. */
	HRESULT SetUp_Shader_UVAnim();

protected:
	ANIM_INFO* m_pAnimInfo = { nullptr };
	_uint m_iAnimTypeNum = { 0 };
	_uint m_iAnimType = { 0 };
	_double m_AnimAcc = { 0.f };
	_bool m_bIsLoop = { false };

	_uint	m_iUVTextureIndex = { 0 }; 
	_uint	m_iUVTexNumX = { 0 }; // UV용 텍스처 가로 개수
	_uint	m_iUVTexNumY = { 0 }; // UV용 텍스처 세로 개수

protected:
	void Free() override;
};

