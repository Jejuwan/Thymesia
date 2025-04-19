#pragma once

#include "Base.h"
#include "physx/PxPhysicsAPI.h"

using namespace physx;

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Find_Component(const wstring& strComTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

protected:
	ID3D11Device*			m_pDevice = { nullptr }; 
	ID3D11DeviceContext*	m_pContext = { nullptr };
	PxRigidDynamic*			m_pRigidBody = { nullptr };

protected:
	map<wstring, class CComponent*>			m_Components;
	_int m_iLayerIdx = { 0 };
	BoundingOrientedBox m_BoundingBox;
	_bool m_bEnablePhysics = { false };
	_bool m_bDead = { false };
	_float m_fRimStrength = 0.f;
	_float4 m_fRimColor = { 0,0,0,1 };
	_float m_TexcoordV = 0.f;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& strComTag, CComponent** ppOut, void* pArg = nullptr);

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

public:
	_int GetLayerIdx() { return m_iLayerIdx; }
	void SetLayerIdx(_int i) { m_iLayerIdx = i; }
	BoundingOrientedBox* Get_Bounding_Box() { return &m_BoundingBox; }
	class CComponent* Get_Component(const wstring& strComTag);
	class PxRigidDynamic* Get_RigidBody() { return m_pRigidBody; }

	_bool* GetPhysics() { return &m_bEnablePhysics; }
	_bool GetDead() { return m_bDead; }
	void UpdateRimStrength(_float4 color, _float fLimit, _float fTimeDelta)
	{
		m_fRimColor = color;

		if (m_fRimStrength < 0.f)
			m_fRimStrength = 0.f;

		if (m_fRimStrength < fLimit || fTimeDelta < 0)
			m_fRimStrength += fTimeDelta;

	}
	_float GetRimStrength() { return m_fRimStrength; }
};

END