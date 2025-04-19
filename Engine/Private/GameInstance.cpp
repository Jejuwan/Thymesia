#include "Engine_Pch.h"
#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Target_Manager.h"
#include "SoundManager.h"
#include "Picking.h"
#include "Physics.h"
#include "File.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::Get_Instance() }
	, m_pInput_Device { CInput_Device::Get_Instance() }
	
	, m_pLevel_Manager { CLevel_Manager::Get_Instance() }
	, m_pObject_Manager { CObject_Manager::Get_Instance() }
	, m_pComponent_Manager { CComponent_Manager::Get_Instance() }
	, m_pTimer_Manager { CTimer_Manager::Get_Instance()}	
	, m_pPipeLine { CPipeLine::Get_Instance() }
	, m_pPicking {CPicking::Get_Instance() }
	, m_pPhysics{ CPhysics::Get_Instance() }
	, m_pLight_Manager{ CLight_Manager::Get_Instance() }
	, m_pTarget_Manager { CTarget_Manager::Get_Instance() }
	, m_pSound_Manager{ CSound_Manager::Get_Instance()}
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pPhysics);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	m_pPhysics->InitPhysics(true);

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	CFile::Get_Instance()->Initialize();

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iSizeX, GraphicDesc.iSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_DInput(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;	

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager )
		return;

	/* 키보드와 마우스의 상태를 저장한다. */
	m_pInput_Device->Update_DInput();
	m_pInput_Device->Tick();

	m_pPhysics->Tick();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();

	m_pObject_Manager->Late_Tick(fTimeDelta);
	m_pLevel_Manager->Late_Tick(fTimeDelta);

	m_pSound_Manager->Tick(fTimeDelta);
}

void CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;

	m_pLevel_Manager->Render();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

ID3D11Device* CGameInstance::Get_Device()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->GetDevice();
}

IDXGISwapChain* CGameInstance::Get_SwapChain()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->GetSwapChain();
}

ID3D11DeviceContext* CGameInstance::Get_Context()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->GetContext();
}

_byte CGameInstance::Get_DIKeyState(_ubyte ubyKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(ubyKeyID);	
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseID);


}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);	
}

_bool CGameInstance::Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Down(eMouseID);
}

_bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Pressing(eMouseID);
}

_bool CGameInstance::Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Up(eMouseID);
}

_bool CGameInstance::Keyboard_Down(_int eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Keyboard_Down(eMouseID);
}

_bool CGameInstance::Keyboard_Pressing(_int eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Keyboard_Pressing(eMouseID);
}

_bool CGameInstance::Keyboard_Up(_int eMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Keyboard_Up(eMouseID);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	return m_pLevel_Manager->Open_Level(iLevelIndex, pLevel);	
}

CLayer* CGameInstance::Get_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);	
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_GameObject(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Get_Player()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Player();
}

void CGameInstance::Set_Player(CGameObject* _player)
{
	return m_pObject_Manager->Set_Player(_player);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{

	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Find_Prototype(iLevelIndex, strPrototypeTag);
}

HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);	
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
	
	return m_pPipeLine->Get_Transform_Matrix(eState);	
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Cursor_Screen(POINT point)
{
	return m_pPicking->SetCursorScreen(point);
}

_bool CGameInstance::Intersect_Ray(BoundingOrientedBox* bSphere, _float* pDist)
{
	if(nullptr == m_pPicking)
		return _bool();
	return m_pPicking->Intersect_Ray(bSphere, pDist);
}

CGameObject* CGameInstance::Intersect_Ray_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pPicking)
		return nullptr;
	return m_pPicking->Intersect_Ray_Layer(iLevelIndex, strLayerTag);
}

_float3 CGameInstance::Get_Create_Pos(_float fDist)
{
	return m_pPicking->Get_Create_Pos(fDist);
}

void CGameInstance::Add_Actor(CTransform* transform, physx::PxRigidDynamic** rigid)
{
	if (nullptr == m_pPhysics)
		return;
	m_pPhysics->createStack(transform, rigid);
}

void CGameInstance::Physics_Clear()
{
	if (nullptr == m_pPhysics)
		return;
	m_pPhysics->Clear();
}

PxPhysics* CGameInstance::GetPhysics()
{
	return m_pPhysics->GetPhysics();
}

PxScene* CGameInstance::GetScene()
{
	return m_pPhysics->GetScene();
}

PxMaterial* CGameInstance::GetMaterial()
{
	return m_pPhysics->GetMaterial();
}

//PxCooking* CGameInstance::GetCooking()
//{
//	return m_pPhysics->GetCooking();
//}

PxController* CGameInstance::CreateController(PxCapsuleControllerDesc* desc)
{
	return m_pPhysics->CreateController(desc);
}

PxControllerManager* CGameInstance::GetControllerManager()
{
	return m_pPhysics->GetControllerManager();
}

PxObstacleContext* CGameInstance::GetObstacleContext()
{
	return m_pPhysics->GetObstacleContext();
}

LIGHTDESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(pDevice, pContext, LightDesc);
}

HRESULT CGameInstance::Clear_Lights()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	
	m_pLight_Manager->Clear_Lights();
	return S_OK;
}

HRESULT CGameInstance::Bind_ShaderResource_RTV(CShader* pShader, const wstring& strTargetTag, const char* pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

HRESULT CGameInstance::Play_Sound(TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->PlaySound(pSoundKey, eID, fVolume);
	return S_OK;
}

HRESULT CGameInstance::PlayBGM(TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->PlayBGM(pSoundKey, eID, fVolume);
	return S_OK;
}

HRESULT CGameInstance::StopSound(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->StopSound(eID);
	return S_OK;
}

HRESULT CGameInstance::StopBGM(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->StopBGM(eID);
	return S_OK;
}

HRESULT CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->StopAll();
	return S_OK;
}

HRESULT CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
	return S_OK;
}

_bool CGameInstance::IsPlaying(CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	return m_pSound_Manager->IsPlaying(eID);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CPhysics::Get_Instance()->Destroy_Instance();

	CFile::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CSound_Manager::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPicking);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPhysics);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pSound_Manager);
}
