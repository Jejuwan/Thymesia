#pragma once

extern int g_Pointer;

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };
		HWND			hWnd;
		unsigned int	iSizeX, iSizeY;
		WINMODE			eWinMode;

	}GRAPHICDESC;



	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;

	typedef struct tagModelMaterial
	{
		class CTexture* pMaterialTexture[AI_TEXTURE_TYPE_MAX];

	}MODEL_MATERIAL;

	typedef struct tagKeyFrame
	{
		XMFLOAT4		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT4		vTranslation;
		float			fTime;

		void Serialize(const char* pSaveFilePath)
		{
			ofstream os(pSaveFilePath, ios::binary | ios::app);
			os.write((char*)&vScale, sizeof(_float4));
			os.write((char*)&vRotation, sizeof(_float4));
			os.write((char*)&vTranslation, sizeof(_float4));
			os.write((char*)&fTime, sizeof(_float));
			os.close();
		}

		void Deserialize(const char* pSaveFilePath)
		{
			ifstream is(pSaveFilePath, ios::binary);
			is.seekg(g_Pointer);
			is.read((char*)&vScale, sizeof(_float4));
			is.read((char*)&vRotation, sizeof(_float4));
			is.read((char*)&vTranslation, sizeof(_float4));
			is.read((char*)&fTime, sizeof(_float));
			g_Pointer = (int)is.tellg();
		}

	}KEYFRAME;

	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		const static unsigned int					iNumElements = { 2 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexPositionPointSize
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		const static unsigned int					iNumElements = { 2 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSPSIZE;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		const static unsigned int					iNumElements = { 3 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		const static unsigned int					iNumElements = { 4 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

		void Serialize(const char* pSaveFilePath)
		{
			ofstream os(pSaveFilePath, ios::binary | ios::app);
			os.write((char*)&vPosition, sizeof(_float3));
			os.write((char*)&vNormal, sizeof(_float3));
			os.write((char*)&vTexcoord, sizeof(_float2));
			os.write((char*)&vTangent, sizeof(_float3));
			os.close();
		}

		void Deserialize(tagVertexMesh& mesh, const char* pSaveFilePath)
		{
			ifstream is(pSaveFilePath, ios::binary);
			is.seekg(g_Pointer);
			is.read((char*)&mesh.vPosition, sizeof(_float3));
			is.read((char*)&mesh.vNormal, sizeof(_float3));
			is.read((char*)&mesh.vTexcoord, sizeof(_float2));
			is.read((char*)&mesh.vTangent, sizeof(_float3));
			g_Pointer = (int)is.tellg();
		}
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* 이 정점은 어떤 인덱스의 뼈를 이용하는가?! */
		XMFLOAT4		vBlendWeights;
		const static unsigned int					iNumElements = { 6 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

		void Serialize(const char* pSaveFilePath)
		{
			ofstream os(pSaveFilePath, ios::binary | ios::app);
			os.write((char*)&vPosition, sizeof(_float3));
			os.write((char*)&vNormal, sizeof(_float3));
			os.write((char*)&vTexcoord, sizeof(_float2));
			os.write((char*)&vTangent, sizeof(_float3));
			os.write((char*)&vBlendIndices, sizeof(XMUINT4));
			os.write((char*)&vBlendWeights, sizeof(_float4));
			os.close();
		}

		void Deserialize(tagVertexAnimMesh& out,const char* pSaveFilePath)
		{
			ifstream is(pSaveFilePath, ios::binary);
			is.seekg(g_Pointer);
			is.read((char*)&out.vPosition, sizeof(_float3));
			is.read((char*)&out.vNormal, sizeof(_float3));
			is.read((char*)&out.vTexcoord, sizeof(_float2));
			is.read((char*)&out.vTangent, sizeof(_float3));
			is.read((char*)&out.vBlendIndices, sizeof(XMUINT4));
			is.read((char*)&out.vBlendWeights, sizeof(_float4));
			g_Pointer = (int)is.tellg();
		}
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagModelDesc
	{
		wstring strPath;
		_float4x4 initWorldMat;
	}MODELDESC;

	typedef struct ENGINE_DLL tagVertexRectInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT4		vRotDir;

		const static unsigned int					iNumElements = { 7 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL tagVertexPointInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		XMFLOAT4		vRotRight;
		XMFLOAT4		vRotUp;
		XMFLOAT4		vRotLook;
		XMFLOAT4		vRotTranslation;
		XMFLOAT2		vRotDir;
		const static unsigned int					iNumElements = { 11 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXPOINTINSTANCE;


	typedef struct ENGINE_DLL tagVertexMeshInstance
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;

		const static unsigned int					iNumElements = { 8 };
		const static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];

	}VTXMESHINSTANCE;



}


