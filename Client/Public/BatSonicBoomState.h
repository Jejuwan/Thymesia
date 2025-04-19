#pragma once
#include "BatState.h"
class CBatSonicBoomState :
    public CBatState
{
public:
	CBatSonicBoomState();
	virtual ~CBatSonicBoomState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	_bool m_bTurn = { false };
	_int m_iAttackCount = 2;
};

