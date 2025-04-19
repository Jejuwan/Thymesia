#pragma once
#include "BatState.h"
BEGIN(ENGINE)
class CTransform;
END

class CBatIntroState :
	public CBatState
{
public:
	CBatIntroState();
	virtual ~CBatIntroState();

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
	_bool m_bUpdate = { false };
	_bool  m_bInitTick = false;
};