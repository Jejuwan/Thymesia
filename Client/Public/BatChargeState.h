#pragma once
#include "BatState.h"
class CBatChargeState :
    public CBatState
{
public:
	CBatChargeState();
	virtual ~CBatChargeState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;
};

