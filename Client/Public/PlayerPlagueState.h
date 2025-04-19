#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerPlagueState :
	public CPlayerState
{
public:
	CPlayerPlagueState(ANIM_TYPE type);
	virtual ~CPlayerPlagueState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

private:
	ANIM_TYPE m_eType;
	_bool m_bAnimChange = { false };
};
END