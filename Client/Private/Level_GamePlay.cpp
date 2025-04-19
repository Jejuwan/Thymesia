#include "Client_Pch.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera_Free.h"
#include "Camera_Player.h"

#include "MapToolUI.h"
#include "FileSystem.h"

#include "Client_Struct.h"
#include "UI.h"
#include "Transform.h"
#include "Layer.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{	
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL; 

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;	

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	CFileSystem::Get_Instance()->Load_Game(TEXT("../../SaveFiles/Maps/Lv1.map"));
	
	CGameInstance::Get_Instance()->PlayBGM(L"Sea_of_Trees_Level_-_Exploration.wav", SOUND_BGM0, 1.f);
	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if(!m_bLogoAlpha && m_FadeScreen != nullptr)
	{
		m_FadeScreen->Set_Alpha(-fTimeDelta * 1.f);

		CLayer* pLayer = pGameInstance->Get_Layer(LEVEL_BOSS_BAT, L"Layer_UI");
		if (m_FadeScreen->Get_Alpha() < 0.01f)
		{
			pGameInstance->Delete_GameObject(LEVEL_BOSS_BAT, L"Layer_UI", pLayer->size() - 1);
			m_FadeScreen = nullptr;
		}
	}

	/*if (pGameInstance->Keyboard_Down(VK_LEFT))
	{
		pGameInstance->Physics_Clear();

		pGameInstance->Clear(LEVEL_BOSS_BAT);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS_BAT))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}*/
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{

		UIDESC desc;
		desc.texName = TEXT("FadeScreen");
		desc.eType = ORTHOGRAPHIC;
		desc.bVisible = true;
		desc.bLookAt = true;
		desc.vScale = _float2(g_iWinSizeX, g_iWinSizeY);
		desc.vPos = _float3(g_iWinSizeX / 2, g_iWinSizeY / 2, 0);
		desc.fAngle = 0.f;
		pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, L"Layer_UI",
			TEXT("Prototype_GameObject_UI"), &desc);


		CLayer* pLayer = pGameInstance->Get_Layer(LEVEL_BOSS_BAT, L"Layer_UI");
		m_FadeScreen = static_cast<CUI*>(pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, L"Layer_UI", pLayer->size() - 1));
		m_FadeScreen->RealSet_Alpha(0.f);
		m_bLogoAlpha = true;
	}

	if (m_bLogoAlpha)
		m_FadeScreen->Set_Alpha(fTimeDelta * 1.f);

	if (m_bLogoAlpha && m_FadeScreen->Get_Alpha() > 0.99f)
	{
		m_fTime += fTimeDelta;
	}

	if (m_fTime > 1.f && !m_bExecuted[0])
	{
		m_bExecuted[0] = true;
		//pGameInstance->StopSound(SOUND_ENVIRONMENT1);
		pGameInstance->Play_Sound(L"Fortress_GateOpen.wav", SOUND_ENVIRONMENT1, 0.7f);
	}

	if (m_fTime > 4.f)
	{
		pGameInstance->Physics_Clear();

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS_BAT))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Clear_Lights();

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHTDESC::TYPE_POINT;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(0.6f, 0.6f, 0.6f, 1.f);

	LightDesc.vPosition = _float4(0, 0, 0, 1);
	LightDesc.fRange = 3.f;

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//CCamera_Free::CAMERAFREEDESC		CameraFreeDesc;

	////CameraFreeDesc.iData = 10;
	//CameraFreeDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	//CameraFreeDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	//CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	//CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	//CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//CameraFreeDesc.CameraDesc.fNear = 0.2f;
	//CameraFreeDesc.CameraDesc.fFar = 300.f;
	//CameraFreeDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	//CameraFreeDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	///* For.Camera */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Camera_Free"), &CameraFreeDesc)))
	//	return E_FAIL;

	CCamera_Player::CAMERAPLAYERDESC		CameraPlayerDesc;

	//CameraFreeDesc.iData = 10;
	CameraPlayerDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	CameraPlayerDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	CameraPlayerDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraPlayerDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraPlayerDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraPlayerDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraPlayerDesc.CameraDesc.fNear = 0.2f;
	CameraPlayerDesc.CameraDesc.fFar = 300.f;
	CameraPlayerDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraPlayerDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Camera_Player"), &CameraPlayerDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance); 

	SPAWNDESC pSpawnDesc;
	pSpawnDesc.m_eCurLevel = LEVEL_GAMEPLAY;

	/* For.Player */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
		TEXT("Prototype_GameObject_Player"), &pSpawnDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	/* For.Monster */
	/*for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
			TEXT("Prototype_GameObject_Gardener"))))
			return E_FAIL;
	}*/
	Safe_Release(pGameInstance);

	return S_OK; 
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Cube"))))
	//	return E_FAIL;

	/* For.Terrain */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	///* For.SKy */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	

	///* For.Effect */

	//for (size_t i = 0; i < 30; i++)
	//{
	//	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//		TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;

	//}

	
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_float fHpPosX = 100;
	_float fHpPosY = g_iWinSizeY - 80;
	_float fMonsterHpPosY = g_iWinSizeY - 70;
	_float fSlotPosX = g_iWinSizeX - 100;
	_float fSlotPosY = g_iWinSizeY - 100;
	_float HpBarScaleX = 250.f;

	UIDESC desc;
	desc.texName = TEXT("TexUI_HPBar_1Border");
	desc.vScale = _float2(26, 12);
	desc.vPos = _float3(fHpPosX, 1000, 0.5f);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_2BG");
	desc.vScale = _float2(HpBarScaleX, 12);
	desc.vPos = _float3(fHpPosX + HpBarScaleX*0.5f, fHpPosY, 0.5f);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_1Border");
	desc.vScale = _float2(26, 12);
	desc.vPos = _float3(fHpPosX + HpBarScaleX, fHpPosY, 0.5f);
	desc.fAngle = 180.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_3MainBar");
	desc.vScale = _float2(HpBarScaleX, 8);
	desc.vPos = _float3(fHpPosX + HpBarScaleX*0.5f, fHpPosY, 0.2);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	//monster

	_float monHpX = 1.25f;
	_float monHpY = 0.04f;

	desc.texName = TEXT("TexUI_HPBar_3BarBorder");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX+0.05f, monHpY + 0.02f);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_2BarHead");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_StunnedShine1");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_3BarBorder");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX + 0.05f, monHpY + 0.02f);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_2BarHead");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_StunnedShine1");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;


	desc.texName = TEXT("TexUI_HPBar_3BarBorder");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX + 0.05f, monHpY + 0.02f);
	desc.vPos = _float3(198, fMonsterHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_2BarHead");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_HPBar_StunnedShine1");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(monHpX, monHpY);
	desc.vPos = _float3(198, fHpPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;


	desc.texName = TEXT("TexUI_PW_PlunderSlotNone");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = true;
	desc.bLookAt = false;
	desc.vScale = _float2(256.f, 256.f);
	desc.vPos = _float3(fSlotPosX, fSlotPosY, 0.0f);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	//plague weapon icons

	desc.texName = TEXT("TexUI_PW_PlunderSlotAxe");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = false;
	desc.vScale = _float2(256.f, 256.f);
	desc.vPos = _float3(fSlotPosX, fSlotPosY, 0.2);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_PW_PlunderSlotHammer");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = false;
	desc.vScale = _float2(256.f, 256.f);
	desc.vPos = _float3(fSlotPosX, fSlotPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_PW_PlunderSlotKnife");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = false;
	desc.vScale = _float2(256.f, 256.f);
	desc.vPos = _float3(fSlotPosX, fSlotPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("TexUI_PW_PlunderSlotScythe");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = false;
	desc.vScale = _float2(256.f, 256.f);
	desc.vPos = _float3(fSlotPosX, fSlotPosY, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("Eff_GlitterA");
	desc.eType = PERSPECTIVE;
	desc.bVisible = false;
	desc.bLookAt = true;
	desc.vScale = _float2(0.5f, 0.7f);
	desc.vPos = _float3(0, 0, 5);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("FadeScreen");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = true;
	desc.bLookAt = true;
	desc.vScale = _float2(g_iWinSizeX*10, g_iWinSizeY*10);
	desc.vPos = _float3(g_iWinSizeX/2, g_iWinSizeX/2, 0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	CLayer* pLayer = pGameInstance->Get_Layer(LEVEL_BOSS_BAT, strLayerTag);
	m_FadeScreen = static_cast<CUI*>(pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, strLayerTag, pLayer->size() - 1));
	Safe_Release(pGameInstance);
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);	

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_GamePlay");
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}
