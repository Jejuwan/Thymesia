#pragma once
#include "BatState.h"
class CBatStunState :
    public CBatState
{
public:
	CBatStunState();
	virtual ~CBatStunState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	_bool m_bTurn = { false };
};

