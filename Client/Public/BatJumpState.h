#pragma once
#include "BatState.h"
class CBatJumpState :
    public CBatState
{
public:
	CBatJumpState();
	virtual ~CBatJumpState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	ANIM_TYPE m_eType;
};

