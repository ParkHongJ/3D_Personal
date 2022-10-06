#pragma once
#include "Base.h"

BEGIN(Engine)
union COLLIDER_ID
{
	struct {
		_uint Left_ID;
		_uint Right_ID;
	};
	LONGLONG ID;
};

class CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)
public:
	enum COLLISIONGROUP { PLAYER = 0, MONSTER, COLLISION_END };
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT Add_CollisionGroup(COLLISIONGROUP eCollisionGroup, class CCollider * pGameObject);
	HRESULT Collision(COLLISIONGROUP eSourGroup, COLLISIONGROUP eDestGroup, _float fTimeDelta);
	HRESULT Clear_CollisionGroup();
private:
	list<class CCollider*>				m_CollisionObjects[COLLISION_END];
	typedef list<class CCollider*>		COLLISIONOBJECTS;
	unordered_map<LONGLONG, _bool>		m_ColInfo;

public:
	virtual void Free() override;
};

END