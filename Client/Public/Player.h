#pragma once

#include "Client_Defines.h"
#include "Client_Struct.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CMonster;
class CPlayer final : public CGameObject,
	public PxUserControllerHitReport
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	enum WEAPON_TYPE { NONE, DEFAULT, AXE, KNIFE, HAMMER, SCYTHE, WEAPON_END };
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

private:
	CModel* m_pModelCom = { nullptr };
	CModel* m_pWeaponLModelCom[WEAPON_END];
	CModel* m_pWeaponRModelCom[WEAPON_END];
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pNonAnimShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pWeaponLTransformCom = { nullptr };
	CTransform* m_pWeaponRTransformCom = { nullptr };
	CTransform* m_pShadowTransformCom = { nullptr };

	PxController* m_pController = { nullptr };
	PxShape* m_pAttackShape = { nullptr };
	PxRigidStatic* m_pAttackActor = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_WeaponL_ShaderResources();
	HRESULT Bind_WeaponR_ShaderResources();

private:
	class CPlayerState* m_CurState = { nullptr };
	class CCamera_Player* m_pCameraPlayer = { nullptr };
	HITDESC m_HitDesc;

	_float m_fHp = 100;
	_bool m_bControllerHit = { false };
	_bool m_bLockon = { false };
	_int m_iWeaponR_Boneidx = { 0 };
	WEAPON_TYPE m_eWeaponType = WEAPON_TYPE::DEFAULT;
	
	LEVEL m_eCurLevel;
	CMonster* m_pTargetMonster = { nullptr };
	_int m_iStealType = 2;

	SPAWNDESC m_pSpawnDesc;
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;

public:
	CModel* Get_Model() { return m_pModelCom; }
	PxController* Get_Controller() { return m_pController; }
	PxRigidStatic* Get_AttackActor() { return m_pAttackActor; }

public:
	CPlayerState* Get_PlayerState() { return m_CurState; }
	_bool Get_ControllerHit() { return m_bControllerHit; }
	void Set_ControllerHit(_bool b) { m_bControllerHit = b; }
	class CCamera_Player* Get_PlayerCamera() { return m_pCameraPlayer; }
	void Set_PlayerCamera(class CCamera_Player* cam) { m_pCameraPlayer = cam; }
	HITDESC& Get_HitDesc() { return m_HitDesc; }
	void Set_Weapon(WEAPON_TYPE type) { m_eWeaponType = type; }
	CMonster* Get_TargetMonster() { return m_pTargetMonster; }
	void Set_TargetMonster(CMonster* mon) { m_pTargetMonster = mon; }
	_int Get_StealType() { return m_iStealType; }
	void Set_StealType(_int i) { m_iStealType = i; }
	void Set_Level(LEVEL level) { m_eCurLevel = level; }
	_float Get_Hp() { return m_fHp; }
	void Set_Hp(_float fHp) { m_fHp = fHp; }
	SPAWNDESC Get_SpawnDesc() { return m_pSpawnDesc; }
public:
	void LookLockOn(class CTransform* _monTrans);
public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) {};
	virtual void onControllerHit(const PxControllersHit& hit);
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) {};
};

END
