#pragma once
#include "BatState.h"
BEGIN(ENGINE)
class CTransform;
END

class CBatAttackState :
	public CBatState
{
public:
	CBatAttackState();
	virtual ~CBatAttackState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;

private:
	ANIM_TYPE m_eType;
};