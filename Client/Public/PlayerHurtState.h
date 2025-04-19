#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerHurtState :
	public CPlayerState
{
public:
	CPlayerHurtState();
	virtual ~CPlayerHurtState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

private:
	ANIM_TYPE m_eHurtType = HURT_S;
	_bool m_bPause = { false };
	_float m_fPauseTime = 0.f;
};
END

