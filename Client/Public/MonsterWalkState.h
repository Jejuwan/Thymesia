#pragma once
#include "MonsterState.h"

BEGIN(Client)
class CMonsterWalkState :
	public CMonsterState
{
public:
	CMonsterWalkState();
	virtual ~CMonsterWalkState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	_float m_fStepSoundTime = -1.f;
};
END

