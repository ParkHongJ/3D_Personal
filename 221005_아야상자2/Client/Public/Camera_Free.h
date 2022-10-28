#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CCollider;
END
BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	_float3 GetNormalizeDir(_uint eState);

	class CTransform* GetTargetTransform();
	HRESULT Set_Player(class CTransform* pPlayerTransform);
	HRESULT Set_Target(class CTransform* pTargetTransform);

	void ReleaseTarget();
private:
	class CTransform* m_pTargetTransform = nullptr;
	class CTransform* m_pPlayerTransform = nullptr;

	CCollider*		  m_pColliderCom = nullptr;
	_float3 m_vCamPosition;

	_float3	m_vPivot;
public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END