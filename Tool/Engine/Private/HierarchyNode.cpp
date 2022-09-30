#include "..\Public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{

}

HRESULT CHierarchyNode::Initialize(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth, Node* pNode)
{
	strcpy_s(m_szName, pAINode->mName.data);


	strcpy_s(pNode->mName, pAINode->mName.data);
	memcpy(&pNode->mTransformation, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_Transformation, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_Transformation, XMMatrixTranspose(XMLoadFloat4x4(&m_Transformation)));

	m_iDepth = iDepth;
	m_pParent = pParent;

	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CHierarchyNode::Set_CombinedTransformation()
{
	if (nullptr != m_pParent)
	{
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	}
	else
	{
		m_CombinedTransformation = m_Transformation;
	}

	m_OldTransformation = m_Transformation;
}

void CHierarchyNode::Set_OffsetMatrix(_fmatrix OffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
}

CHierarchyNode * CHierarchyNode::Create(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth, Node* pNode)
{
	CHierarchyNode*			pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pAINode, pParent, iDepth, pNode)))
	{
		MSG_BOX(TEXT("Failed To Created : CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}
