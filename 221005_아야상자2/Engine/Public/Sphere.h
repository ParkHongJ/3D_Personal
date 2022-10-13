#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CSphere final : public CCollider
{
protected:
	CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

public:
	BoundingSphere Get_Collider() {
		return *m_pSphere;
	}

public:
	HRESULT Add_CollisionGroup(CCollider_Manager::COLLISIONGROUP eCollisionGroup, class CCollider* pCollider);
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual _bool Picking(_fvector vRayPos, _fvector vRayDir, _float fDist);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingSphere*			m_pSphere = nullptr;
	BoundingSphere*			m_pOriginal_Sphere = nullptr;

public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END