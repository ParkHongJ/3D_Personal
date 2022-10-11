#include "..\Public\Collider_Manager.h"
#include "Collider.h"
#include "GameObject.h"
IMPLEMENT_SINGLETON(CCollider_Manager)

CCollider_Manager::CCollider_Manager()
{

}
HRESULT CCollider_Manager::Add_CollisionGroup(COLLISIONGROUP eCollisionGroup, CCollider * pGameObject)
{
	m_CollisionObjects[eCollisionGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);
	return S_OK;
}
HRESULT CCollider_Manager::Collision(COLLISIONGROUP eSourGroup, COLLISIONGROUP eDestGroup, _float fTimeDelta)
{
	if (m_CollisionObjects[eDestGroup].empty() || m_CollisionObjects[eSourGroup].empty())
		return S_OK;

	unordered_map<LONGLONG, bool>::iterator iter;
	
	for (auto& pSour : m_CollisionObjects[eSourGroup])
	{
		for (auto& pDest : m_CollisionObjects[eDestGroup])
		{
			COLLIDER_ID ID;
			ID.Left_ID = pSour->GetID();
			ID.Right_ID = pDest->GetID();
			iter = m_ColInfo.find(ID.ID);
			
			//충돌 정보가 미등록 상태라면
			if (m_ColInfo.end() == iter)
			{
				//등록해주고 다시찾음
				m_ColInfo.insert(make_pair(ID.ID, false));
				iter = m_ColInfo.find(ID.ID);
			}
			
			_bool bCollision = pSour->Collision(pDest);
			CGameObject* pSourGameObject = pSour->GetOwner();
			CGameObject* pDestGameObject = pDest->GetOwner();

			if (bCollision)
			{
				//현재 충돌 중이다
				if (iter->second)
				{
					pSourGameObject->OnCollisionStay(pDestGameObject, fTimeDelta);
					pDestGameObject->OnCollisionStay(pSourGameObject, fTimeDelta);
				}
				else
				{
					//현재 처음 충돌이다
					pSourGameObject->OnCollisionEnter(pDestGameObject, fTimeDelta);
					pDestGameObject->OnCollisionEnter(pSourGameObject, fTimeDelta);
					iter->second = true;
				}
			}
			else
			{
				//이전에 충돌하고 있었다
				if (iter->second)
				{
					pSourGameObject->OnCollisionExit(pDestGameObject, fTimeDelta);
					pDestGameObject->OnCollisionExit(pSourGameObject, fTimeDelta);
					iter->second = false;
				}
			}
		}
	}
	return S_OK;
}
HRESULT CCollider_Manager::Clear_CollisionGroup()
{
	for (auto& List : m_CollisionObjects)
	{
		for (auto& pGameObject : List)
		{
			Safe_Release(pGameObject);
		}
		List.clear();
	}
	return S_OK;
}
void CCollider_Manager::Free()
{
}
