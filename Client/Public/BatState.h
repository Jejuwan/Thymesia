#pragma once
#include "Base.h"
#include "MonsterState.h"
BEGIN(Engine)
class CTransform;
class CModel;
class CGameInstance;
END

BEGIN(Client)
class CBatState :
	public CMonsterState, PxControllerFilterCallback
{
public:
	CBatState();
	virtual ~CBatState();

	virtual CMonsterState* Transition(class CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(class CMonster& _player, _float fTimeDelta);
	virtual void Exit(class CMonster& _player);
	virtual void Free() override;

	void PhysicsUpdate(CMonster& _player, _float fTimeDelta);
	void RootPosUpdate(CMonster& _player, _float fAmount, _float fTimeDelta);
protected:
	class CPlayer* m_pPlayer = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pCameraTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

protected:
	enum ANIM_TYPE {
		INTRO, IDLE, WALK,ATTACK_L,ATTACK_R,ATTACK_R_HARD,
		BITE1,BITE2,CHARGE,SONICBOOM_BEFORE,SONICBOOM,FLYSCREAM,STUNLOOP,STUNSTART,DIE, JUMPATK,JUMPATK_INPLACE,
		TURN_L,TURN_R
	};
	enum ATTACK_TYPE {
		ATTACK, BITE, CHRAGE, JUMP, BOOM
	};

	static class CBat* m_pBat;
	class CTransform* m_pWeaponLTransform = nullptr;
	class CTransform* m_pWeaponRTransform = nullptr;

	static _uint m_iAttackIdx;

	_float m_fAmountZ = -2.f;
	_float m_fAmountY = 0.f;
	_float m_fLRAngle = 0.f;
	_float m_fLRAngleDegree = 0.f;
	_float m_fTurnAngle = 0.f;
	_float m_fTurnTime = 0.f;
	_bool m_bControllerCollide = { true };
	_bool m_bExecuted[10] = { false };
	_bool m_bHitRim = { false };
	_float m_fHitCoolTime = 0.5f;

	static _int m_iAttackCount;
	static _int m_iChargeCount;

public:
	_bool Get_ControllerHit() { return m_bControllerHit; }
	void Set_ControllerHit(_bool b) { m_bControllerHit = b; }
	_bool Get_Attack() { return m_bAttack; }
	string Get_Name() { return m_szName; }
	void Set_Bat(class CBat* pBat) { m_pBat = pBat; }

	virtual bool filter(const PxController& a, const PxController& b);
};
END

