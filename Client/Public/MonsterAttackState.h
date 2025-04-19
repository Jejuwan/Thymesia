#pragma once
#include "MonsterState.h"

BEGIN(Client)
class CMonsterAttackState :
	public CMonsterState
{
public:
	CMonsterAttackState();
	virtual ~CMonsterAttackState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

	ANIM_TYPE m_eType;
};
END

