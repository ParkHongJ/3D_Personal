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

	typedef struct tagVertexModelInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMODELINSANCE;

	typedef struct ENGINE_DLL tagVertexModelInstance_Declaration
	{
		static const unsigned int iNumElements = 8;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODELINSTANCE_DECLARATION;
	/* 인스턴싱으로 그려지는 각각의 인스턴스 각각의
	로컬영역에서의 상태를 표현하기위한 행렬정보를 포함한다. */
	typedef struct tagVertexInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		/* 내가 그릴려고 했던 정점(VTXTEX)과 해당 모델의 로컬 상탤르 ㄹ표현하는
		VTXINSTANCE의 정보가 함께 셰이더로 전달되어야한다. */
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE_DECLARATION;

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
		XMFLOAT4		vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertexColor_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCOL_DECLARATION;

	typedef struct tagVertexPoint
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vSize;
	}VTXPOINT;

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
		std::vector<FACEINDICES32> mFaces;
		std::vector<Bone> mBones;
	}MESH;

	typedef struct NodeAnim {
		char mNodeName[MAX_PATH] = "";
		unsigned int mNumKeyFrames;
		std::vector<KEYFRAME> mKeyFrames;
	}NodeAnim;

	typedef struct Animation {
		char mName[MAX_PATH] = "";
		float mDuration;
		float mTickPerSecond;
		unsigned int mNumChannels;
		std::vector<NodeAnim> mChannels;
	}ANIMATION;

	typedef struct Node {
		char mName[MAX_PATH] = "";
		unsigned int mNumChildren;
		std::vector<Node> mChildren;
		XMFLOAT4X4 mTransformation;
	}NODE;

	typedef struct Material {
		wchar_t mName[MAX_PATH] = L"";
		unsigned int TextureType;
	}MATERIAL;

	typedef struct HScene {
		unsigned int mNumMeshes;
		unsigned int mNumMaterials;
		unsigned int mNumAnimations;
		unsigned int mNumNewMaterials;
		NODE mRootNode;
		std::vector<ANIMATION> mAnimations;
		std::list<Material> mMaterials;
		std::vector<std::vector<Material>> mNewMaterials;
		std::vector<Mesh> mMesh;
	}HSCENE;
}
