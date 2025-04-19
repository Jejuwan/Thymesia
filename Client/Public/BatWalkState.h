#pragma once
#include "BatState.h"
BEGIN(ENGINE)
class CTransform;
END

class CBatWalkState :
	public CBatState
{
public:
	CBatWalkState();
	virtual ~CBatWalkState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	_float m_fStepSoundTime = -1.f;
};