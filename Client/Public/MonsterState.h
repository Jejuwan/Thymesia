#pragma once
#include "Base.h"
#include "Monster.h"
BEGIN(Engine)
class CTransform;
class CModel;
class CGameInstance;
END

BEGIN(Client)
class CMonsterState :
    public CBase
{
public:
	CMonsterState();
	virtual ~CMonsterState();

	virtual CMonsterState* Transition(class CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(class CMonster& _player, _float fTimeDelta);
	virtual void Exit(class CMonster& _player);
	virtual void Free() override;

	void PhysicsUpdate(CMonster& _player,_float fTimeDelta);
	void RootPosUpdate(CMonster& _player,_float fAmount, _float fTimeDelta);
protected:
	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	enum ANIM_TYPE {
		IDLE, WALK, ATK1, ATK2, ATK3, HURT_L, HURT_S1, HURT_S2, DIE, STUNSTART, STUNLOOP, STUNDIE
	};

	PxControllerCollisionFlags m_CollisionFlag;
	_bool m_bSideColided = { false };

	class CGameInstance* m_pGameInstance = { nullptr };
	string m_szName = "";
	_bool m_bExecuted[10] = { false };

	_float4 m_vPreRootPos;
	_float4 m_vInitPos = { 0,0,0,1 };
	_float3 m_vPrePos = { 0,0,0 };
	_bool	m_bEnd = false;
	_bool   m_bInterpolate = false;
	_bool   m_bControllerHit = false;
	_bool   m_bAttack = false;
	_bool   m_bRotStart = false;
	_float m_fTime = 0.f;
	_vector m_vPreDir = { 0,0,0 };
	_vector m_vCurDir = { 0,0,0 };
	_float m_RotTime = 0.f;
	_float m_fDuration = 0.f;
	_float m_fTrackPos = 0.f;
	_bool m_bInitHit = false;
	_bool m_bHitRim = false;
	_float m_fHPBarX;
	_bool m_bStunBarShine = false;

public:
	_bool Get_ControllerHit() { return m_bControllerHit; }
	void Set_ControllerHit(_bool b) { m_bControllerHit = b; }
	_bool Get_Attack() {return m_bAttack;}
	void Set_Attack(_bool b) { m_bAttack = b; }
	string Get_Name() { return m_szName; }
};
END

