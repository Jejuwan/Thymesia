#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerPlunderState :
	public CPlayerState
{
public:
	CPlayerPlunderState();
	virtual ~CPlayerPlunderState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

private:
	_bool m_bPlunderHit = { false };
};
END