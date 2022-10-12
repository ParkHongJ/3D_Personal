#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CModel;
class CHierarchyNode;
END

BEGIN(Client)

class CRas_Samrah final : public CGameObject
{
public:
	enum PARTTYPE { PART_WEAPON, PART_END };

	enum COLLIDERTYPE { COLLIDERTYPE_AABB, COLLIDERTYPE_OBB, COLLIDERTYPE_SPHERE, COLLILDERTYPE_END };

private:
	CRas_Samrah(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRas_Samrah(const CRas_Samrah& rhs);
	virtual ~CRas_Samrah() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta) override;

private:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr };

private:
	CGameObject*			m_Parts = nullptr;

	vector<class CHierarchyNode*>		m_Sockets;


private:
	HRESULT Ready_Sockets();
	HRESULT Ready_Parts();

	HRESULT Update_Weapon();
	HRESULT Ready_Components();

public:
	static CRas_Samrah* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END