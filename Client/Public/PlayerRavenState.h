#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerRavenState :
	public CPlayerState
{
public:
	CPlayerRavenState();
	virtual ~CPlayerRavenState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

	_bool m_bHoldButton = { true };
	_bool m_bAnimChange = { false };
};
END