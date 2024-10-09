#pragma once
#include "Base.h"
#include "Engine_Defines.h"
#include "Collider.h"
#include <set>
BEGIN(Engine)

class CCollision_Manager : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

private:
	union COLLIDER_ID
	{
		struct
		{
			_uint iSourID;
			_uint iDestID;
		};
		ULONGLONG ID;
	};

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

public:
	void	Clear_Collider();
	HRESULT	Attach_Collider(const _uint LayerBitset, CCollider* pCollider);
	void	Collision_Matrix();

	void	Set_CollisionLayer(const _uint LayerBitset, const _uint CollisionLayersBitset = (1 << 0))
	{
		auto LayerIter = m_CollisionLayerMap.find(LayerBitset);

		if (m_CollisionLayerMap.end() == LayerIter)
		{
			m_CollisionLayerMap.emplace(LayerBitset, CollisionLayersBitset);
		}
		else
		{
			LayerIter->second = CollisionLayersBitset;
		}
	}

private:
	void	Update_Collision(list<CCollider*> pDestCol, list<CCollider*> pSourCol);

private:
	// @note - map은 key값을 기준으로 오름차순 정렬 (greater<자료형>: 내림차순 정렬)
	// @note - map은 레드블랙트리, 검색 O(log N), 삽입/삭제 O(log N), 중복키X
	// @note - AB, BA처럼 같은 묶음에 대하여 두 번 검사하지 않도록 Layer 삽입 시 정렬을 하여 AB로 보관 및 검사하도록.

	// 1. Add_CollisionLayer 시 <Layer, 교유bitset> 으로 저장  (흐아.. 레이어를 enum으로 정의해ㅓㅅ 고유 bitset 지정해줘얗마 !!)
	// 2. for문으로 돌 때 m_CollisionLayerMap.find(Souriter->first);로 bitset 받아옴. (없으면 CollisionLayer 목록에 없는 것! 
	// 3. if(CollisionLayer & m_CollisionLayerMap.find(DestIter->first)이면 충돌 가능 레이어
	map<_uint, _uint>						m_CollisionLayerMap;
	map<_uint, list<CCollider*>>			m_ColliderMap;
	map<ULONGLONG, _bool>					m_CollisionMap;
	_uint				m_iLayerNum;

public:
	virtual void Free() override;
};

END