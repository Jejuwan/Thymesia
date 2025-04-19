#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayer;
class CPlayerIdleState :
    public CPlayerState
{
public:
	CPlayerIdleState();
	virtual ~CPlayerIdleState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;
};
END

