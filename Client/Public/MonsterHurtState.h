#pragma once
#include "MonsterState.h"
BEGIN(Client)
class CMonsterHurtState :
	public CMonsterState
{
public:
	CMonsterHurtState(ANIM_TYPE type);
	virtual ~CMonsterHurtState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

	ANIM_TYPE m_eHurtType = HURT_S1;
};
END

