#pragma once
#include "MonsterState.h"
BEGIN(Client)
class CMonsterDeadState :
	public CMonsterState
{
public:
	CMonsterDeadState();
	virtual ~CMonsterDeadState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;
};
END

