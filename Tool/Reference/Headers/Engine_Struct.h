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
		std::vector<VertexWeight> mWeights;
		XMFLOAT4X4 mOffsetMatrix;
	}BONE;

	typedef struct VerticesInfo {
		//mNumVertices만큼 갖고있음
		XMFLOAT3 mVertices;
		XMFLOAT3 mNormals;
		XMFLOAT2 mTextureCoords;
		XMFLOAT3 mTangents;
	}VERTICESINFO;
	typedef struct Mesh {
		char mName[MAX_PATH] = "";
		unsigned int mMaterialIndex;
		unsigned int mNumVertices;
		unsigned int mNumFaces;
		unsigned int mNumBones;
		std::vector<VerticesInfo> mVertices;
		//std::vector<VTXANIMMODEL> mAnimVertices;
		std::vector<FACEINDICES32> mFaces;
		std::vector<Bone> mBones;
		//Bone** mBones;
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
		/*unsigned int mNumScalingKeys;
		unsigned int mNumRotationKeys;
		unsigned int mNumPositionKeys;*/
		unsigned int mNumKeyFrames;
		std::vector<KEYFRAME> mKeyFrames;
		/*VectorKey* mScalingKeys;
		QuatKey* mRotationKeys;
		VectorKey* mPositionKeys;*/
	}NodeAnim;

	typedef struct Animation {
		char mName[MAX_PATH] = "";
		float mDuration;
		float mTickPerSecond;
		unsigned int mNumChannels;
		//NodeAnim** mChannels;
		std::vector<NodeAnim> mChannels;
	}ANIMATION;

	typedef struct Node {
		char mName[MAX_PATH] = "";
		unsigned int mNumChildren;
		//Node* mParent;
		//Node** mChildren;
		std::vector<Node> mChildren;
		XMFLOAT4X4 mTransformation;
	}NODE;

	typedef struct Material {
		wchar_t mName[MAX_PATH] = L"";
		unsigned int TextureType;
	}MATERIAL;

	typedef struct TempScene {
		unsigned int mNumMeshes;
		unsigned int mNumMaterials;
		unsigned int mNumAnimations;
		unsigned int mNumNewMaterials;
		NODE mRootNode;
		//ANIMATION** mAnimations;
		std::vector<ANIMATION> mAnimations;
		std::list<Material> mMaterials;
		std::vector<std::vector<Material>> mNewMaterials;
		std::vector<Mesh> mMesh;
	}TEMPSCENE;

	typedef struct MyTest {

	}MYTEST;
}