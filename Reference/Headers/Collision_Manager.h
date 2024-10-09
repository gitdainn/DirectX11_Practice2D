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
	HRESULT	Attach_Collider(const _tchar* pLayer, CCollider* pCollider);
	void	Collision_Matrix();
	HRESULT	Add_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB);
	// HRESULT Remove_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB);

private:
	void	Update_Collision(list<CCollider*> pDestCol, list<CCollider*> pSourCol);
	bool	Is_IgnoreLayer(const _tchar* pLayerA, const _tchar* pLayerB) const;

private:
	// @note - map은 key값을 기준으로 오름차순 정렬 (greater<자료형>: 내림차순 정렬)
	// @note - map은 레드블랙트리, 검색 O(log N), 삽입/삭제 O(log N), 중복키X
	// @note - AB, BA처럼 같은 묶음에 대하여 두 번 검사하지 않도록 Layer 삽입 시 정렬을 하여 AB로 보관 및 검사하도록.
	map<const _tchar*, list<CCollider*>>		m_ColliderMap;
	map<ULONGLONG, _bool>						m_CollisionMap;
	set<const _tchar*>	m_IgnoreLayerMap;
	_uint				m_iLayerNum;

public:
	virtual void Free() override;
};

END