#pragma once
#include "BatState.h"
class CBatScreamState :
    public CBatState
{
public:
	CBatScreamState();
	virtual ~CBatScreamState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;
};

