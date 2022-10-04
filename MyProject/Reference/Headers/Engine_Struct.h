#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		float		fTime;

		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;		
	}KEYFRAME;

	/* 빛의 정보를 담아놓기 위한 구조체. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_END };

		TYPE			eType;

		XMFLOAT4		vDirection;

		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;

	typedef struct tagMaterialDesc
	{
		class CTexture*		pTexture[AI_TEXTURE_TYPE_MAX];
	}MATERIALDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	// D3DDECLUSAGE
	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;		
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* 이 정점에 영향을 주는 뼈의 인덱스 네개. */
		XMFLOAT4		vBlendWeight; /* 영향르 주고 있는 각 뼈대의 영향 비율 */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;


	typedef struct tagVertexColor
	{
		XMFLOAT3		vPosition;
		unsigned long	dwColor;
	}VTXCOL;

	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		WINMODE eWinMode;

	}GRAPHICDESC;



	typedef struct VertexWeight {
		unsigned int mVertexId;
		float mWeight;
	}VERTEXWEIGHT;

	typedef struct Bone {
		char mName[MAX_PATH] = "";
		unsigned int mNumWeights;
		VertexWeight* mWeights;
		XMFLOAT4X4 mOffsetMatrix;
		~Bone()
		{
			delete mWeights;
		}
	}BONE;

	typedef struct Face {
		unsigned int* mIndices;
		Face()
		{
			mIndices = new unsigned int[3];
		}
		~Face()
		{
			delete[] mIndices;
		}
	}FACE;

	typedef struct Mesh {
		char mName[MAX_PATH] = "";
		unsigned int mMaterialIndex;
		unsigned int mNumVertices;
		unsigned int mNumFaces;
		unsigned int mNumBones;

		//mNumVertices만큼 갖고있음
		XMFLOAT3* mVertices;
		XMFLOAT3* mNormals;
		XMFLOAT3* mTangents;
		XMFLOAT3* mTextureCoords[0x8];

		Bone** mBones;
		Face* mFaces;

		~Mesh()
		{
			delete[] mVertices;
			delete[] mNormals;
			delete[] mTangents;
			delete[] mTextureCoords[0];
			/*for (unsigned int i = 0; i < mNumVertices; ++i)
			{
			}*/
			
			for (unsigned int i = 0; i < mNumBones; ++i)
			{
				delete[] mBones[i];
			}
			delete mBones;


			delete[] mFaces;
			/*for (unsigned int i = 0; i < mNumFaces; ++i)
			{
				delete &mFaces[i];
			}*/
		}
	}MESH;

	typedef struct VectorKey
	{
		double mTime;
		XMFLOAT3 mValue;
	}VECTORKEY;

	typedef struct QuatKey
	{
		double mTime;
		XMFLOAT4 mValue;
	}QUATKEY;

	typedef struct NodeAnim {
		char mNodeName[MAX_PATH] = "";
		unsigned int mNumScalingKeys;
		unsigned int mNumRotationKeys;
		unsigned int mNumPositionKeys;
		VectorKey* mScalingKeys;
		QuatKey* mRotationKeys;
		VectorKey* mPositionKeys;

		~NodeAnim()
		{
			delete[] mScalingKeys;
			delete[] mRotationKeys;
			delete[] mPositionKeys;
		}
	}NodeAnim;

	typedef struct Animation {
		char mName[MAX_PATH] = "";
		float mDuration;
		float mTickPerSecond;
		unsigned int mNumChannels;
		NodeAnim** mChannels;
		~Animation() {
			for (unsigned int i = 0; i < mNumChannels; ++i)
			{
				delete[] & mChannels[i];
			}
			delete[] mChannels;
		}
	}ANIMATION;

	typedef struct Node {
		char mName[MAX_PATH] = "";
		unsigned int mNumChildren;
		Node** mChildren;
		XMFLOAT4X4 mTransformation;

		~Node() {
			for (unsigned int i = 0; i < mNumChildren; ++i)
			{
				delete[] mChildren[i];
			}
			delete mChildren;
		}
		
		
	}NODE;

	typedef struct Material {
		wchar_t mName[MAX_PATH] = L"";
		unsigned int TextureType;
	}MATERIAL;

	typedef struct TempScene {
		unsigned int mNumMeshes;
		unsigned int mNumMaterials;
		unsigned int mNumAnimations;
		Node* mRootNode;
		Mesh* mMesh;
		ANIMATION** mAnimations;
		std::list<Material*> mMaterials;
		
		//void DeleteNode(Node* pNode)
		//{
		//	/*delete pNode;
		//	for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
		//	{
		//		DeleteNode(pNode->mChildren[i]);
		//	}*/
		//}
		void Free()
		{
			delete mRootNode;
			/*delete[] mMesh;


			for (auto& iter : mMaterials)
			{
				delete iter;
			}
			mMaterials.clear();


			if (nullptr != mAnimations)
			{
				for (unsigned int i = 0; i < mNumAnimations; ++i)
				{
					delete[] mAnimations[i];
				}
				delete[] mAnimations;
			}*/
			
		}


		unsigned int m_dwRefCnt = 0;

		unsigned long Release()
		{
			if (0 == m_dwRefCnt)
			{
				int a = 10;

				Free();

				delete this;

				return 0;
			}
			else
				return m_dwRefCnt--;
		}

		unsigned long AddRef()
		{
			return ++m_dwRefCnt;
		}

		

	}TEMPSCENE;
}


//HANDLE		hFile = CreateFile(L"../Bin/Resources/Meshes/Fiona/LEVEL_8.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//
//	if (INVALID_HANDLE_VALUE == hFile)
//		return E_FAIL;
//
//	//읽은 바이트 
//	DWORD	dwByte = 0;
//	DWORD	dwStrByte = 0;
//
//	WriteFile(hFile, &m_TempScene->mNumMeshes, sizeof(_uint), &dwByte, nullptr);
//	WriteFile(hFile, &m_TempScene->mNumMaterials, sizeof(_uint), &dwByte, nullptr);
//	WriteFile(hFile, &m_TempScene->mNumAnimations, sizeof(_uint), &dwByte, nullptr);
//
//	//Mesh저장
//	for (_uint i = 0; i < m_TempScene->mNumMeshes; ++i)
//	{
//		Mesh* pMesh = &m_TempScene->mMesh[i];
//		//이름저장
//		dwStrByte = DWORD(sizeof(char) * strlen(pMesh->mName));
//		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
//		WriteFile(hFile, &pMesh->mName, dwStrByte, &dwByte, nullptr);
//
//		WriteFile(hFile, &pMesh->mMaterialIndex, sizeof(_uint), &dwByte, nullptr);
//		WriteFile(hFile, &pMesh->mNumVertices, sizeof(_uint), &dwByte, nullptr);
//		WriteFile(hFile, &pMesh->mNumFaces, sizeof(_uint), &dwByte, nullptr);
//		WriteFile(hFile, &pMesh->mNumBones, sizeof(_uint), &dwByte, nullptr);
//
//		//Vertices저장
//		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
//		{
//			WriteFile(hFile, &pMesh->mVertices[j], sizeof(XMFLOAT3), &dwByte, nullptr);
//			WriteFile(hFile, &pMesh->mNormals[j], sizeof(XMFLOAT3), &dwByte, nullptr);
//			WriteFile(hFile, &pMesh->mTangents[j], sizeof(XMFLOAT3), &dwByte, nullptr);
//		}
//
//		//TextureCoord저장
//		for (_uint j = 0; j < 8; ++j)
//		{
//			WriteFile(hFile, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT3), &dwByte, nullptr);
//		}
//		
//		//Bone저장
//		for (_uint j = 0; j < pMesh->mNumBones; ++j)
//		{
//			Bone* pBone = pMesh->mBones[j];
//			
//			//BoneName저장
//			dwStrByte = DWORD(sizeof(char) * strlen(pBone->mName));
//			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
//			WriteFile(hFile, &pBone->mName, dwStrByte, &dwByte, nullptr);
//
//			//OffsetMatrix저장
//			WriteFile(hFile, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

			//NumWeights저장
//			WriteFile(hFile, &pBone->mNumWeights, sizeof(_uint), &dwByte, nullptr);//			

//			for (_uint k = 0; k < pBone->mNumWeights; ++k)
//			{
//				//VertexWeight저장
//				WriteFile(hFile, &pBone->mWeights[k].mVertexId, sizeof(_uint), &dwByte, nullptr);
//				WriteFile(hFile, &pBone->mWeights[k].mWeight, sizeof(_float), &dwByte, nullptr);
//			}
//		}
//
//		//Face저장
//		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
//		{
//			WriteFile(hFile, &pMesh->mFaces[j].mIndices[0], sizeof(_uint), &dwByte, nullptr);
//			WriteFile(hFile, &pMesh->mFaces[j].mIndices[1], sizeof(_uint), &dwByte, nullptr);
//			WriteFile(hFile, &pMesh->mFaces[j].mIndices[2], sizeof(_uint), &dwByte, nullptr);
//		}
//	}
//
//	//Material저장
//	for (_uint i = 0; i < m_TempScene->mNumMaterials; ++i)
//	{
//		//Texture경로 저장
//		Material* pMaterial = m_TempScene->mMaterials.front();
//		dwStrByte = DWORD(sizeof(_tchar) * wcslen(pMaterial->mName));
//		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
//		WriteFile(hFile, &pMaterial->mName, dwStrByte, &dwByte, nullptr);
//
//		WriteFile(hFile, &pMaterial->TextureType, sizeof(_uint), &dwByte, nullptr);
//		
//		Safe_Delete(m_TempScene->mMaterials.front());
//		m_TempScene->mMaterials.pop_front();
//	}
//
//	//Aniamtion저장
//	for (_uint i = 0; i < m_TempScene->mNumAnimations; ++i)
//	{
//		Animation* pAnimation = m_TempScene->mAnimations[i];
//
//		//AnimationName저장
//		dwStrByte = DWORD(sizeof(char) * strlen(pAnimation->mName));
//		WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
//		WriteFile(hFile, &pAnimation->mName, dwStrByte, &dwByte, nullptr);
//
//		//Animation정보저장
//		WriteFile(hFile, &pAnimation->mDuration, sizeof(_float), &dwByte, nullptr);
//		WriteFile(hFile, &pAnimation->mTickPerSecond, sizeof(_float), &dwByte, nullptr);
//		WriteFile(hFile, &pAnimation->mNumChannels, sizeof(_uint), &dwByte, nullptr);
//
//		for (_uint j = 0; j < pAnimation->mNumChannels; ++j)
//		{
//			NodeAnim*	pNodeAnim = pAnimation->mChannels[j];
//
//			//NodeName저장
//			dwStrByte = DWORD(sizeof(char) * strlen(pNodeAnim->mNodeName));
//			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
//			WriteFile(hFile, &pNodeAnim->mNodeName, dwStrByte, &dwByte, nullptr);
//
//			//NumPosition저장
//			WriteFile(hFile, &pNodeAnim->mNumPositionKeys, sizeof(_uint), &dwByte, nullptr);
//
//			//PositionKey저장
//			for (_uint k = 0; k < pNodeAnim->mNumPositionKeys; ++k)
//			{
//				WriteFile(hFile, &pNodeAnim->mPositionKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
//				WriteFile(hFile, &pNodeAnim->mPositionKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
//			}
//
//			//NumRotation저장
//			WriteFile(hFile, &pNodeAnim->mNumRotationKeys, sizeof(_uint), &dwByte, nullptr);
//
//			//RotationKey저장
//			for (_uint k = 0; k < pNodeAnim->mNumRotationKeys; ++k)
//			{
//				WriteFile(hFile, &pNodeAnim->mRotationKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
//				WriteFile(hFile, &pNodeAnim->mRotationKeys[k].mValue, sizeof(XMFLOAT4), &dwByte, nullptr);
//			}
//
//			//NumScale저장
//			WriteFile(hFile, &pNodeAnim->mNumScalingKeys, sizeof(_uint), &dwByte, nullptr);
//			
//			//ScalingKey저장
//			for (_uint k = 0; k < pNodeAnim->mNumScalingKeys; ++k)
//			{
//				WriteFile(hFile, &pNodeAnim->mScalingKeys[k].mTime, sizeof(_double), &dwByte, nullptr);
//				WriteFile(hFile, &pNodeAnim->mScalingKeys[k].mValue, sizeof(XMFLOAT3), &dwByte, nullptr);
//			}
//		}
//	}
//	CloseHandle(hFile);
//	return S_OK;