#include "Client_Pch.h"
#include "..\Public\Level_MapTool.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera_Free.h"
#include "MapToolUI.h"


CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	CMapToolUI::Get_Instance()->Tick(fTimeDelta);
	m_bUITicked = true;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if(pGameInstance->Keyboard_Down(VK_RIGHT))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}
}

void CLevel_MapTool::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
}

HRESULT CLevel_MapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if(m_bUITicked)
	{
		CMapToolUI::Get_Instance()->Render();
	}

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	LIGHTDESC		LightDesc;

	/* 방향성 광원을 추가한다. */
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCamera_Free::CAMERAFREEDESC		CameraFreeDesc;

	//CameraFreeDesc.iData = 10;
	CameraFreeDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	CameraFreeDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
	CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraFreeDesc.CameraDesc.fNear = 0.2f;
	CameraFreeDesc.CameraDesc.fFar = 300.f;
	CameraFreeDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraFreeDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Camera */
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_MAPTOOL, strLayerTag,
		TEXT("Prototype_GameObject_Camera_Free"), &CameraFreeDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Cube"))))
	//	return E_FAIL;

	/* For.Terrain */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, strLayerTag,
	//	TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	///* For.SKy */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//	TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;

}

HRESULT CLevel_MapTool::Ready_Layer_Effect(const wstring& strLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	///* For.Effect */

	//for (size_t i = 0; i < 30; i++)
	//{
	//	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, strLayerTag,
	//		TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;

	//}

	return S_OK;
}

CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_MapTool");
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();


}
