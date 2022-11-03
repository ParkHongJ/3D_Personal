#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Yantari final : public CLevel
{
private:
	CLevel_Yantari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Yantari() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ForkLift(const _tchar* pLayerTag);
	HRESULT Ready_Layer_GameObject(const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg = nullptr);

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

	HRESULT Load();
private:
	list<CREATE_INFO> m_CreateObj;

public:
	static CLevel_Yantari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END