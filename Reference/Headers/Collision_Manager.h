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
	union ID
	{
		struct
		{
			_uint iSourID;
			_uint iDestID;
		};
		ULONGLONG ID;
	};

	struct DUAL_ID
	{
		DUAL_ID(const _tchar* pLayerA, const _tchar* pLayerB)
		{
			if (pLayerA > pLayerB)
			{
				pSourLayer = pLayerB;
				pDestLayer = pLayerA;
			}
			else
			{
				pSourLayer = pLayerA;
				pDestLayer = pLayerB;
			}
		}

		const _tchar* pSourLayer = { nullptr };
		const _tchar* pDestLayer = { nullptr };
	};

	struct COLLIDER_INFO
	{
		const _tchar* pSourLayer;
		const _tchar* pDestLayer;
		CCollider* pSourCollider;
		CCollider* pDestCollider;
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
	HRESULT	Attach_Collider(const _tchar* pLayerTag, CCollider* pCollider);
	void	Set_CollisionLayerMatrix(const _tchar* pLayerDest, const _tchar* pLayerSour, const _bool bIsCollision);

private:
	void	Update_Collision(list<CCollider*> pSourColList, list<CCollider*> pDestColList, const _tchar* pSourLayer, const _tchar* pDestLayer);
	/** 충돌이 true인 상태인데 더이상 Collision_Manager에 추가되지 않아 
	onCollisionExit가 호출되지 않은 콜라이더들을 추적하여 Exit 처리합니다.*/
	void	Track_ExitCollision();

	template<typename T>
	struct Sorting
	{
		void operator()(T& Sour, T& Dest)
		{
			if (Sour > Dest)
			{
				T Temp = Sour;
				Sour = Dest;
				Dest = Temp;
			
			}
			return;
		}
	};



private:
	// @note - map은 key값을 기준으로 오름차순 정렬 (greater<자료형>: 내림차순 정렬)
	// @note - map은 레드블랙트리, 검색 O(log N), 삽입/삭제 O(log N), 중복키X
	// @note - AB, BA처럼 같은 묶음에 대하여 두 번 검사하지 않도록 Layer 삽입 시 정렬을 하여 AB로 보관 및 검사하도록.

	// 1. Add_CollisionLayer 시 <Layer, 교유bitset> 으로 저장  (흐아.. 레이어를 enum으로 정의해ㅓㅅ 고유 bitset 지정해줘얗마 !!)
	// 2. for문으로 돌 때 m_CollisionLayerMap.find(Souriter->first);로 bitset 받아옴. (없으면 CollisionLayer 목록에 없는 것! 
	// 3. if(CollisionLayer & m_CollisionLayerMap.find(DestIter->first)이면 충돌 가능 레이어
	// map<_uint, _uint>						m_CollisionLayerMap;
	map<const _tchar*, list<CCollider*>>	m_ColliderMap;
	map<ULONGLONG, _bool>					m_CollisionColliderMap;
	map<ULONGLONG, COLLIDER_INFO>			m_CollisionTrueColliderInfoMap; // 충돌 상태가 true인 모든 콜라이더 정보 보관
	set<ULONGLONG>			m_CurrentTickDualIDSet;

	unordered_map<const _tchar*, _uint>				m_LayerIDMap;
	unordered_map<ULONGLONG, _bool>					m_CollisionLayerMap; // 아이디 조합, 두 개의 충돌 여부
	_uint									m_iLayerNum = { 0 };
	_uint									m_iLayerID = { 0 };

public:
	virtual void Free() override;
};

END