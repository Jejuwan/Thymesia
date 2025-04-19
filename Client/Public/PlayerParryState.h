#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerParryState :
	public CPlayerState
{
public:
	CPlayerParryState(ANIM_TYPE type);
	virtual ~CPlayerParryState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

	ANIM_TYPE m_eType;
};
END

