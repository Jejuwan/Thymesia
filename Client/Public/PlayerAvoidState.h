#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerAvoidState :
	public CPlayerState
{
public:
	CPlayerAvoidState(ANIM_TYPE type);
	virtual ~CPlayerAvoidState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

	ANIM_TYPE m_eType = IDLE;
};
END

