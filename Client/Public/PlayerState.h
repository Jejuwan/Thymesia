#pragma once
#include "Base.h"
BEGIN(Engine)
class CTransform;
class CModel;
class CGameInstance;
END

BEGIN(Client)
class CPlayerState :
	public CBase
{
public:
	CPlayerState();
	virtual ~CPlayerState();

	virtual CPlayerState* Transition(class CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(class CPlayer& _player,_float fTimeDelta);
	virtual void Exit(class CPlayer& _player);
	virtual void Free() override;

	void PhysicsUpdate(CPlayer& _player, _float fTimeDelta);
	void RootPosUpdate(CPlayer& _player, _float fAmount, _float fTimeDelta);

protected:
	PxControllerCollisionFlags m_CollisionFlag;
	_bool m_bSideColided = { false };

	CGameInstance* m_pGameInstance = { nullptr };
	string m_szName = "";

	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pCameraTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_float4 m_vInitPos = { 0,0,0,1 };
	_float3 m_vPrePos = { 0,0,0 };
	_bool	m_bEnd = false;
	_bool   m_bInterpolate = false;
	_bool   m_bAttack = false;
	_bool	m_bAttackChange = false;
	_bool   m_bRotStart = false;
	_float m_fTime = 0.f;
	_float m_fTrackPos = 0.f;
	_float m_fDuration = 0.f;

	_bool m_bBatExecute = { false };
	_bool m_bExecuted[10] = { false };
	_bool m_bRimChange = { false };
	_int m_iAttackIndex = 4;

protected:
	enum ANIM_TYPE {
		IDLE, WALK_F, WALK_FL, WALK_FR, WALK_B, WALK_BL, WALK_BR, WALK_L, WALK_R,
		AVOID_F, AVOID_B, AVOID_L, AVOID_R, HURT_L, HURT_S, ATK1, ATK2, ATK3, ATK4, ATK5,
		PARRY1, PARRY2, CROW1, CROW2, PLUNDER, AXE1, AXE2, HAMMER1, HAMMER2, KNIFE, SCYTHE1, SCYTHE2, 
		FALLDOWN, FALLDOWN_END, HEADACHE_START, HEADACHE_LOOP, HEADACHE_END, BAT_EXECUTE, DIE
	};

public:
	string Get_Name() { return m_szName; }
	_bool Get_Attack() { return m_bAttack; }
	void Set_Attack(_bool b) { m_bAttack = b; }
	_bool Get_AttackChange() { return m_bAttackChange; }
	void Set_AttackChange(_bool b) { m_bAttackChange = b; }
	void Set_BatExecute(_bool b) { m_bBatExecute = b; }
	_int Get_AttackIndex() { return m_iAttackIndex; }
};
END
