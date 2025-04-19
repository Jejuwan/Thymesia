#pragma once
#include "BatState.h"
class CBatDeadState :
    public CBatState
{
public:
	CBatDeadState();
	virtual ~CBatDeadState();

	virtual CMonsterState* Transition(CMonster& _player);
	virtual void Entry(CMonster& _player);
	virtual void Update(CMonster& _player, _float fTimeDelta);
	virtual void Exit(CMonster& _player);
	virtual void Free() override;
private:
	_vector m_vPreLook;
	_vector m_vNextLook;
	_vector m_vPreCamPos;
	_float fCameraMoveTime = 0.f;
	_bool m_bIntroAnim = { false };
};

