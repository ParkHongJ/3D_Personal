#pragma once
#include "Ras_Hands.h"
class CHand1 : public CRas_Hands
{
public:
	CHand1();
	virtual ~CHand1() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	virtual void OnCollisionEnter(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionStay(CGameObject* pOther, _float fTimeDelta) override;
	virtual void OnCollisionExit(CGameObject* pOther, _float fTimeDelta) override;

	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

