#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	
	
	
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
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
	void Change_Animation(_uint iAnimIndex);
	_uint Get_AnimBoneSize(_uint iAnimIndex);
	void TempFunc(_uint iAnimIndex, _uint iNextAnimIndex);
	_uint GetAnimSize();
	HRESULT SaveFBXToBinary(const _tchar* ModelSavePath);
	void SaveNode(HANDLE hFile, Node* pNode, DWORD& dwByte, DWORD& dwStrByte);
	HRESULT LoadBinary();
	void LoadNode(HANDLE hFile, Node* pNode, DWORD& dwByte, DWORD& dwStrByte);
public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pModelSavePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);


public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex);
	
private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;
	TEMPSCENE*					m_TempScene = nullptr;
	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;
	_bool						m_bClone = false;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;
	_tchar									m_SavePath[MAX_PATH] = L"";
private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	//need save

	vector<wstring>							m_TexturePath;
private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;

private:
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;


private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent, _uint iDepth, Node* pMyNode);
	HRESULT Ready_Animations();

private:
	


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pModelSavePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END