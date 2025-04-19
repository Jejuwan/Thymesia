#include "ToolModelAnim_Pch.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "Cube.h"
//#include "Monster.h"
//#include "Effect.h"
//#include "Sky.h"


#pragma region COMPONENT

#pragma endregion

#pragma region GAMEOBJECT

#pragma endregion

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice) 
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY ThreadEntry(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	/*  모든 컴객체를 초기화하낟. */
	CoInitializeEx(nullptr, 0);

	if (FAILED(pLoader->LoadingForNextLevel()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;	

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;	

	return S_OK;
}

HRESULT CLoader::LoadingForNextLevel()
{
	EnterCriticalSection(&m_CriticalSection);	

	HRESULT		hr = { 0 };

	switch (m_eNextLevel)
	{
	case LEVEL_GAMEPLAY:
		hr = Loading_ForGamePlay();
		break;
	}

	if (FAILED(hr))
		goto except;		

	LeaveCriticalSection(&m_CriticalSection);
	return S_OK;

except:
	LeaveCriticalSection(&m_CriticalSection);
	return E_FAIL;
}


HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default0.dds")))))

		return E_FAIL;
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;	


	///* For.Prototype_Component_Texture_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster"),
	//	CTexture::Create(m_pDevice, pContext, CTexture::TYPE_GENERAL, TEXT("../../Resources/Textures/Monster/Monster.png")))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice, pContext, CTexture::TYPE_CUBE, TEXT("../../Resources/Textures/sKYbOX/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
	//	CTexture::Create(m_pDevice, pContext, CTexture::TYPE_GENERAL, TEXT("../../Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;

	///* For.Prototype_Component_Texture_Filter */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
	//	CTexture::Create(m_pDevice, pContext, CTexture::TYPE_GENERAL, TEXT("../../Resources/Textures/Terrain/Filter.bmp"), 1))))
	//	return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));

	_matrix		PivotMatrix = XMMatrixIdentity();

	//PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../../SaveFiles/a.b"), PivotMatrix))))
	//	return E_FAIL;

	Create_Model("../../Resources/Models/Props/Bat/Lv2_Props");
	//Create_Model_Anim("../../Resources/Animations/Corvus");
	//Create_Model_Anim_Hard("../../Resources/Animations/Bosses/B_Bat");

	/* For.Prototype_Component_Model_Fiona */
	//PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona2"),
	//	/*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
	//	CModel::Deserialize(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../SaveFiles/Map/Circus_Artist_01.map"))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fiona */
	//PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona4"),
	//	/*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
	//	CModel::Deserialize(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../SaveFiles/Map/Circus_Artist_02.map"))))
	//	return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, pContext))))
	//	return E_FAIL;
	//

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"),
		CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, pContext))))
	//	return E_FAIL;


	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pDevice, pContext))))
	//	return E_FAIL;	



	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	Safe_Release(pGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Create_Model(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)/* *XMMatrixRotationX(XMConvertToRadians(-90.0f))*/ *XMMatrixRotationY(XMConvertToRadians(-180.0f));
	for (const auto& entry : directory_iterator(strPath)) {
		string dir = strPath;
		dir.replace(dir.begin(), dir.begin() + 15, "../../SaveFiles");
		if (!exists(dir))
		{
			create_directory(path(dir));
		}
		
		if (is_directory(entry.status()))
			Create_Model(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".FBX") {
			string input = entry.path().string();
			const char* path = input.c_str();

			string fileName = entry.path().filename().string();
 			string protoName = entry.path().filename().stem().string();
			protoName += ".model";
			string saveFileString = dir +"/"+ protoName;
 			const char* saveFilePath = saveFileString.c_str();
			pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, entry.path().wstring(),
				/*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
				CModel::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_NONANIM, path, saveFilePath, PivotMatrix));	
		}
	}
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Create_Model_Anim_Hard(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CModel* pBaseModelCom = nullptr;
	char modelPath[250] = "";

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *XMMatrixRotationX(XMConvertToRadians(-90.0f)) * XMMatrixRotationY(XMConvertToRadians(-180.0f));
	for (const auto& entry : directory_iterator(strPath)) {
		string dir = strPath;
		dir.replace(dir.begin(), dir.begin() + 15, "../../SaveFiles");
		if (!exists(dir))
		{
			create_directory(path(dir));
		}

		_int animCount = 0;
		path curPath = entry.path();
		curPath.remove_filename();

		for (const auto& entry2 : directory_iterator(curPath.string())) {
			if (is_regular_file(entry2) && entry2.path().extension() == ".FBX") {
				animCount++;
			}
		}

		if (is_directory(entry.status()))
			Create_Model(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".FBX") {
			string input = entry.path().string();
			const char* path = input.c_str();

			string fileName = entry.path().filename().string();
			string protoName = entry.path().filename().stem().string();
			protoName += ".anim";
			string saveFileString = dir + "/" + protoName;
			const char* saveFilePath = saveFileString.c_str();

			if (nullptr == pBaseModelCom)
			{
				pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, entry.path().wstring(),
					CModel::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_ANIM, path, saveFilePath, PivotMatrix));
				pBaseModelCom = static_cast<CModel*>(pGameInstance->Get_Prototype(LEVEL_GAMEPLAY, entry.path().wstring()));
				strcpy_s(modelPath, saveFilePath);
				ofstream os(saveFilePath, ios::binary | ios::app);
				os.write((char*)&animCount, sizeof(_uint));
				os.close();
				pBaseModelCom->Serialize_Animations_Hard(modelPath);
			}
			else
			{
				pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, entry.path().wstring(),
					CModel::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_ANIM, path, saveFilePath, PivotMatrix));
				CModel* pAnimModelCom = static_cast<CModel*>(pGameInstance->Get_Prototype(LEVEL_GAMEPLAY, entry.path().wstring()));
				if (exists(saveFilePath))
					remove(saveFilePath);
				pAnimModelCom->Serialize_Animations_Hard(modelPath);
			}
		}
	}
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Create_Model_Anim(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	char modelPath[250] = "";

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.0f)) * XMMatrixRotationY(XMConvertToRadians(-180.0f));
	for (const auto& entry : directory_iterator(strPath)) {
		string dir = strPath;
		dir.replace(dir.begin(), dir.begin() + 15, "../../SaveFiles");
		if (!exists(dir))
		{
			create_directory(path(dir));
		}

		_int animCount = 0;
		path curPath = entry.path();
		curPath.remove_filename();

		for (const auto& entry2 : directory_iterator(curPath.string())) {
			if (is_regular_file(entry2) && entry2.path().extension() == ".FBX") {
				animCount++;
			}
		}

		if (is_directory(entry.status()))
			Create_Model_Anim(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".FBX") {
			string input = entry.path().string();
			const char* path = input.c_str();


			string fileName = entry.path().filename().string();
			string protoName = entry.path().filename().stem().string();
			protoName += ".anim";
			string saveFileString = dir + "/" + protoName;
			const char* saveFilePath = saveFileString.c_str();

			pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, entry.path().wstring(),
				/*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
				CModel::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_ANIM, path, saveFilePath, PivotMatrix));
			CModel* pBaseModelCom = static_cast<CModel*>(pGameInstance->Get_Prototype(LEVEL_GAMEPLAY, entry.path().wstring()));
			strcpy_s(modelPath, saveFilePath);
			pBaseModelCom->Serialize_Animations(modelPath);
		}
	}
	Safe_Release(pGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLoader");
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteObject(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
