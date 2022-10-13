#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

BEGIN(Engine)

class CMeshContainerInstance final : public CVIBuffer
{
private:
	CMeshContainerInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainerInstance(const CMeshContainerInstance& rhs);
	virtual ~CMeshContainerInstance() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance, Mesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_HierarchyNodes(class CModel* pModel, Mesh* pMesh);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

	virtual HRESULT Render();
private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
private:
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */
	vector<class CHierarchyNode*>	m_Bones;

	ID3D11Buffer*			m_pVBInstance = nullptr;
	_uint					m_iInstanceStride;
	_uint					m_iNumInstance;
	_uint					m_iNumIndices;
private:
	HRESULT Ready_Vertices(Mesh* pMesh, _fmatrix PivotMatrix);

public:
	static CMeshContainerInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance, Mesh* pMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END