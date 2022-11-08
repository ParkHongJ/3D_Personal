#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CStaticObject final : public CGameObject
{
private:
	typedef struct CreateObjInfo
	{
		//생성할때 필요한 자료.
		//이름
		//프로토타입 태그
		//레이어 태그
		//레벨
		//모델명
		char szName[260] = "";
		wchar_t pPrototypeTag[260] = L"";
		wchar_t pLayerTag[260] = L"";
		unsigned int iNumLevel = LEVEL_END;
		wchar_t pModelTag[260] = L"";

		XMFLOAT4X4 WorldMatrix;

	}CREATE_INFO;
private:
	CStaticObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticObject(const CStaticObject& rhs);
	virtual ~CStaticObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _bool Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom =  nullptr;

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CStaticObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END