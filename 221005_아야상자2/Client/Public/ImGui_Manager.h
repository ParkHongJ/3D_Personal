#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Layer.h"

BEGIN(Engine)
class CComponent;
END
BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
public:
	enum Tool { MAP, UNIT, CAMERA, PARTICLE, TOOL_END };
public:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	
	void RenderBegin();
	void Render();
	void RenderEnd();

	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv);

	HRESULT AddGameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, _uint iNumLevel, void* pArg = nullptr);
	
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	static vector<pair<string, ID3D11ShaderResourceView*>> resources;

	//test
	_float3	vPosition;
	_float3	vScale;
	_float3	vRotation;

	_uint item_current_idx = 0;

	Tool m_eCurrentTool = TOOL_END;

	/*============
	===Particle===
	============*/
	_bool m_bLooping = false;

	_float m_fDuration = 0.f;
	_float m_fStartDelay = 0.f;
	_float m_fStartLifeTime = 0.f;
	_float m_fStartSpeed = 0.f;

	map<const _tchar*, class CLayer*>* m_pHierarchyList = nullptr;
	class CGameObject* m_pSelectedObject = nullptr;
	map<const _tchar*, class CComponent*>* m_pPrototypeComponent = nullptr;
	map<const _tchar*, class CGameObject*>* m_pPrototypeGameObject = nullptr;
	_matrix	vMatrix;


	_float vPos[3] = {};
	_float vRot[3] = {};
	_float vScal[3] = {};


	
public:
	virtual void Free() override;
};

END

