#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_INSTANCE_NONANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CHierarchyNode* Get_HierarchyNode(const char* pNodeName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_MaterialIndex(_uint iMeshIndex);

	void Set_AnimIndex(_uint iAnimIndex) {
		m_iCurrentAnimIndex = iAnimIndex;
	}

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

	/* 애니메이션에 메세지가 있냐? */
	_bool RecvMessage(const char* Message);

	/* For. AnimationTool */
	vector<class CAnimation*>* GetAnimations();

	void Change_Animation(_uint iAnimIndex, _float fBlendTime = 0.25f, _bool bLoop = true);

	/* For. LoadBinary */
	HRESULT LoadBinary(const _tchar* ModelFilePath, _bool bNewVersion = false);
	void LoadNode(HANDLE hFile, Node* pNode, DWORD& dwByte, DWORD& dwStrByte);
	
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix, _bool bNewVersion = false);
	virtual HRESULT Initialize_Prototype(TYPE eType, const _tchar* pModelFilePath, _uint iNumInstance, _fmatrix PivotMatrix, _bool bNewVersion = false);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	/* 애니메이션을 재생한다. */
	/* 1. 해당 애니메이션에서 사용하는 모든 뼈들의  Transformation 행렬을 갱신한다. */
	/* 2. Transformation를 최상위 부모로부터 자식으로 계속 누적시켜간다.(CombinedTransformation) */
	/* 3. 애니메이션에 의해 움직인 뼈들의 CombinedTransfromation을 셋팅한다. */
	_bool Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

	void SetSpeed(_uint iAnimIndex, _float fSpeed);
private:
	HSCENE*						m_pHScene = nullptr;
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;
	_bool									m_bClone = false;

	//InstanceInfo
	vector<class CMeshContainerInstance*>			m_InstanceMeshes;
	typedef vector<class CMeshContainerInstance*>	INSTANCE_MESHES;
private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_InstanceMeshContainers(_uint iNumInstance, _fmatrix PivotMatrix);
	HRESULT Ready_Materials(_bool bNewVersion = false);
	HRESULT Ready_HierarchyNodes(Node* pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();
	
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bNewVersion = false);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _uint iNumInstance, _fmatrix PivotMatrix = XMMatrixIdentity(), _bool bNewVersion = false);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END