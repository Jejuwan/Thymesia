#pragma once

#include "Component_Manager.h"
#include "Input_Device.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Render_Engine();
	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();
	ID3D11Device* Get_Device();
	IDXGISwapChain* Get_SwapChain();
	ID3D11DeviceContext* Get_Context();

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte ubyKeyID);
	_byte Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID);
	_long Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);
	_bool Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID);
	_bool Keyboard_Down(_int eMouseID);
	_bool Keyboard_Pressing(_int eMouseID);
	_bool Keyboard_Up(_int eMouseID);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);

public: /* For.Object_Manager */
	class CLayer* Get_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Delete_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	class CGameObject* Get_Player();
	void Set_Player(CGameObject* _player);
public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
	CComponent* Get_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: /* For.PipeLine */ 
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState);

	_float4	Get_CamPosition();
public: /* For.Picking */
	void Set_Cursor_Screen(POINT point);
	_bool Intersect_Ray(BoundingOrientedBox* bSphere, _float* pDist);
	class CGameObject* Intersect_Ray_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	_float3 Get_Create_Pos(_float fDist);

public: /* For.Physics */
	void Add_Actor(class CTransform* transform, class PxRigidDynamic** rigid);
	void Physics_Clear();

	_float Get_FadeAlpha() { return m_fFadeAlpha; }
	void Set_FadeAlpha(_float f) { m_fFadeAlpha = f; }


	class PxPhysics* GetPhysics(); 
	class PxScene* GetScene(); 
	class PxMaterial* GetMaterial();
	//class PxCooking* GetCooking(); 
	class PxController* CreateController(PxCapsuleControllerDesc* desc);
	class PxControllerManager* GetControllerManager();
	class PxObstacleContext* GetObstacleContext();

public: /* For.Light_Manager */
	LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Clear_Lights();

public: /* For.Target_Manager */
	HRESULT Bind_ShaderResource_RTV(class CShader* pShader, const wstring& strTargetTag, const char* pConstantName);

public: /* For.Sound_Manager */
	HRESULT Play_Sound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	HRESULT PlayBGM(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	HRESULT StopSound(CHANNELID eID);
	HRESULT StopBGM(CHANNELID eID);
	HRESULT StopAll();
	HRESULT SetChannelVolume(CHANNELID eID, float fVolume);
	_bool	IsPlaying(CHANNELID eID);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CPhysics*					m_pPhysics = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

	_float m_fFadeAlpha = 0.f;

	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END