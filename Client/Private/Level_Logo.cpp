#include "Client_Pch.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Client_Struct.h"
#include "UI.h"
#include "Layer.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{


}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	/* 원형을 복제하여 사본을 만든다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
}

void CLevel_Logo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		m_bLogoAlpha = true;
		pGameInstance->Play_Sound(L"Fantasy_Game_Water_Bolt_3.wav", SOUND_EFFECT, 0.3f);
	} 

	if(m_bLogoAlpha)
		m_FadeScreen->Set_Alpha(fTimeDelta * 1.f);

	if (m_FadeScreen->Get_Alpha() > 0.99f)
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	///* For.BackGround */
	//if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOGO, strLayerTag,
	//	TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	UIDESC desc;
	desc.texName = TEXT("GameLogo2");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = true;
	desc.bLookAt = true;
	desc.vScale = _float2(g_iWinSizeX , g_iWinSizeY / 2);
	desc.vPos = _float3(g_iWinSizeX / 2, g_iWinSizeY / 2, 0.1f);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("FadeScreen");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = true;
	desc.bLookAt = true;
	desc.vScale = _float2(g_iWinSizeX, g_iWinSizeY);
	desc.vPos = _float3(g_iWinSizeX / 2, g_iWinSizeY / 2, 0.2);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, L"Layer_Fade",
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	desc.texName = TEXT("FadeScreen");
	desc.eType = ORTHOGRAPHIC;
	desc.bVisible = true;
	desc.bLookAt = true;
	desc.vScale = _float2(g_iWinSizeX, g_iWinSizeY);
	desc.vPos = _float3(g_iWinSizeX / 2, g_iWinSizeY / 2, 0.0);
	desc.fAngle = 0.f;
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_BOSS_BAT, strLayerTag,
		TEXT("Prototype_GameObject_UI"), &desc)))
		return E_FAIL;

	CLayer* pLayer = pGameInstance->Get_Layer(LEVEL_BOSS_BAT, strLayerTag);
	m_FadeScreen = static_cast<CUI*>(pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, strLayerTag, pLayer->size() - 1));
	m_FadeScreen->RealSet_Alpha(0.f);
	Safe_Release(pGameInstance);
	return S_OK;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);	

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CLevel_Logo");
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();


}
