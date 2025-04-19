#pragma once
#include "BatState.h"
class CBatBiteState :
    public CBatState
{
public:
	CBatBiteState();
	virtual ~CBatBiteState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	ANIM_TYPE m_eType;
};

