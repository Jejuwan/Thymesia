#include "Client_Pch.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "Camera_Player.h"

#include "BackGround.h"
#include "Terrain.h"
#include "Cube.h"
#include "Villager_M.h"
#include "Villager_F.h"
#include "Joker.h"
#include "Gardener.h"
#include "Bat.h"
#include "Bat_BoomBall.h"
#include "Effect.h"
#include "Player.h"
#include "Snow_Red.h"
#include "PlayerHpBar.h"
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
	case LEVEL_LOGO:
		hr = Loading_ForLogo();
		break;
	case LEVEL_MAPTOOL:
		hr = Loading_ForGameMapTool();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_ForGamePlay();
		break;
	case LEVEL_BOSS_BAT:
		hr = Loading_ForBossBat();
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

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

	/* For.Prototype_Component_Texture_Default */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default0.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Tile0.dds")))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Default%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Particles/Blood/Blood%d.dds"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Particles/Smoke/TFX_Smoke_06_8x8.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Particles/Smoke/TFX_Smoke_8x8_02.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Particles/Smoke/TFX_Smoke_8x8_03.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Particles/Smoke/TFX_Smoke_12x12_01.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	Load_Texture("../../Resources/Particles");
	Load_Texture("../../Resources/UI");
	Load_Model("../../SaveFiles/Particles/Mesh");
	Load_Model("../../SaveFiles/Models");
	Load_Model_Anim("../../SaveFiles/Animations/Corvus");
	Load_Model_Anim("../../SaveFiles/Animations/Monsters");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 3000))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_300"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, 600))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Villager_M"),
		CVillager_M::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Villager_F"),
		CVillager_F::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Joker"),
		CJoker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gardener"),
		CGardener::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshAlpha"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMeshAlpha.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Objects*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"),
		CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Texture"),
		CEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle"),
		CSnow_Red::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGameMapTool()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));



	lstrcpy(m_szLoading, TEXT("모델을 로딩 중입니다."));


	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));


	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));



	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로딩 중입니다."));

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

	/* For.Prototype_Component_Model_Fiona */
	//PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Models/Fiona/Fiona.fbx",TEXT("../../asdf.asdf"), PivotMatrix))))
	//	return E_FAIL;

  //	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		///*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
		//CModel::Deserialize(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../SaveFiles/Map/Circus_Artist_03.map"))))
		//return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona2"),
	//	/*CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Models/Fiona/Fiona.fbx", TEXT("../a.b"), PivotMatrix)*/
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,"../../Resources/Models/Fiona/Fiona.fbx","../../SaveFiles/Map/ddong"))))
	//	return E_FAIL;
	///* For.Prototype_Component_VIBuffer_Cube*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, pContext))))
	//	return E_FAIL;
	//

	lstrcpy(m_szLoading, TEXT("셰이더를 로딩 중입니다."));
	///* For.Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;
	//
	///* For.Prototype_Component_Shader_VtxMesh */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	//	return E_FAIL;

	///* For.Prototype_Component_Shader_VtxAnimMesh */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	//	return E_FAIL;

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));
	/* For.Terrain */

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pDevice, pContext))))
	//	return E_FAIL;


	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pDevice, pContext))))
	//	return E_FAIL;	



	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForBossBat()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//Load_Model("../../SaveFiles/Particles/Bat_Boomball");
	Load_Model("../../SaveFiles/Particles/Bat_Boomball");
	Load_Model_Anim("../../SaveFiles/Animations/Bosses/B_Bat");

	lstrcpy(m_szLoading, TEXT("객체원형 로딩 중입니다."));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_Bat"),
		CBat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_B_Bat_Boomball"),
		CBat_BoomBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

void CLoader::Load_Model(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_Model(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".model") {
			string input = entry.path().string();
			const char* path = input.c_str();
			wstring fileStem = entry.path().stem().wstring();
			pGameInstance->Add_Prototype(LEVEL_STATIC, fileStem,
				CModel::Deserialize(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_NONANIM, path));
		}
	}
}

void CLoader::Load_Model_Anim(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_Model_Anim(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".anim") {
			string input = entry.path().string();
			const char* path = input.c_str();
			wstring fileStem = entry.path().stem().wstring();
			pGameInstance->Add_Prototype(LEVEL_STATIC, fileStem,
				CModel::Deserialize(pGameInstance->Get_Device(), pGameInstance->Get_Context(), CModel::TYPE_ANIM, path));
		}
	}
}

void CLoader::Load_Texture(string strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_Model_Anim(entry.path().string());
		else if (is_regular_file(entry) && entry.path().extension() == ".dds") {
			wstring fileStem = entry.path().stem().wstring();
			wstring wstr = TEXT("Prototype_Component_Texture_");
			wstr += fileStem;
			pGameInstance->Add_Prototype(LEVEL_STATIC, wstr,
				CTexture::Create(m_pDevice, m_pContext,entry.path().wstring().c_str()));
		}
	}
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
