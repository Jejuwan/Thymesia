#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCameraPlayerDesc
	{
		CCamera::CAMERADESC			CameraDesc;
	}CAMERAPLAYERDESC;
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

	class CTransform* m_pLockonTransform = { nullptr };
	class CTransform* m_pFixedTransform = { nullptr };
	_bool m_bCameraFixed = { false };
	class CPlayer* m_pPlayer = { nullptr };
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	void Lockon(class CTransform* trans);
	class CTransform* GetLockonTransform() { return m_pLockonTransform; }
	class CTransform* Get_Transform() { return m_pTransform; }
	class CTransform* Get_Fixed_Transform() { return m_pFixedTransform; }
	void Set_Camera_Fixed(_bool b) { m_bCameraFixed = b; }
};

END