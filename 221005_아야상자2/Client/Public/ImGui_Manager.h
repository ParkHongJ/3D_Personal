#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Layer.h"

BEGIN(Engine)
class CComponent;
class CModel;
class CAnimation;
class CCell;
END
BEGIN(Client)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
public:
	enum Tool { TOOL_MAP, TOOL_UNIT, TOOL_CAMERA, TOOL_PARTICLE, TOOL_ANIMATION, TOOL_NAVIGATION, TOOL_END };
	enum NavMesh { NAV_ADD, NAV_EDIT_POINT, NAV_EDIT_CELL, NAV_END };
	typedef struct AnimInfo {
		string name;
		string message;
		//다음 애니메이션의 정보
		_int iNextAnimIndex;
		_float fBlendTime;
		//현재 애니메이션의 정보
		_bool bLoop;
		_bool bHasExitTime;
	}ANIM_INFO;

	typedef struct AnimName {
		string name;
		_uint iNextIndex;
	}ANIM_NAME;

	typedef struct SelectCellPoint {
		//셀의 몇번째 포인트를 피킹했는지
		_uint iPointIndex;
		//진짜 Cell의 본체의 인덱스.
		_uint iOriginCellIndex;
	}SELECT_CELL_POINT;


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
	//typedef struct CreateObjInfo
	//{
	//	//생성할때 필요한 자료.
	//	//이름
	//	//프로토타입 태그
	//	//레이어 태그
	//	//레벨
	//	//모델명
	//	std::string szName;
	//	std::wstring pPrototypeTag;
	//	std::wstring pLayerTag;
	//	std::wstring pModelTag;
	//	unsigned int iNumLevel;
	//}CREATE_INFO;
public:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice , ID3D11DeviceContext* pContext);
	
	void RenderBegin();
	void Render();
	void RenderEnd();

	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv);
	void PushPickPos(_fvector vPickPos);
	void DeletePickPos();
	void ClearPickPos();

	//Load
	HRESULT LoadObject();

	//Cell
	void RenderGizmo();

	HRESULT AddGameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, _uint iNumLevel, void* pArg = nullptr);
	
	/* For NavMesh*/
	void ShowNavMesh();
	void SaveCellIndices();

	/* For Hierarchy */
	void ShowHierarchy();

	/* For Inspector */
	void Inspector();

	wstring CharToWstring(const char* src)
	{
		return wstring(src, src + strlen(src));
	}

	_vector GetRotation(_fmatrix WorldMatrix);
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

	list<CREATE_INFO> m_CreateObj;

	static vector<pair<string, ID3D11ShaderResourceView*>> resources;

	_bool	m_bPicking = false;
	_bool	m_bRenderCell = false;
	//test
	_float3	vPosition;
	_float3	vScale;
	_float4	vRotation;

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
	_float vRot[4] = {};
	_float vScal[3] = {};

	/* For NavMesh */
	_uint m_iCurrentNaviIndex = 0;
	_float3 m_vPickPos[3];
	_uint m_iNumCell = -1;
	_uint m_iNumPointIndex = -1;
	NavMesh m_eNav = NAV_END;
	vector<class CCell*> m_Cells;
	vector<pair<class CCell*, SELECT_CELL_POINT>> m_SelectCellwithPoints;
	vector<pair<class CCell*, SELECT_CELL_POINT>> m_SelectCell;


	/* For Animation */
	class CModel* m_pModel = nullptr;

	/* 현재 오브젝트가 갖고있는 애니메이션의 고유한 인덱스 */
	_uint Animation_current_idx = 0;

	/* 내가 수정하고자 하는 애니메이션의 인덱스 */
	_uint Animation_Edit_Idx = 0;
	_uint Animation_Next_Idx = 0;

	ANIM_NAME SelectedCurrentAnim;
	ANIM_NAME SelectedNextAnim;

	map<string, _uint> m_CurrentAnim;
	map<string, _uint> m_NextAnim;

	vector<pair<ANIM_NAME, vector<ANIM_INFO>>> m_ResultPair;

	//vector<vector<pair<string, _uint>>> m_ResultPair;

	/* 수정하려는 애니메이션의 정보 */
	_bool m_bLoop = false;
	_bool m_bHasExitTime = false;
	_float m_fAnimDuration = 0.0f;
	_float m_fBlendTime = 0.25f;

	char buf[MAX_PATH] = "";

	/* For Inspector */
	vector<class CAnimation*>* m_pAnimations = nullptr;
public:
	virtual void Free() override;
};

END

