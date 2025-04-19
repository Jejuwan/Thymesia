#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerAttackState :
    public CPlayerState
{
public:
	CPlayerAttackState();
	virtual ~CPlayerAttackState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;
	
private:
	_bool m_bHitReset = false;

public:
	_bool Get_HitReset() { return m_bHitReset; }
	void Set_HitReset(_bool b) { m_bHitReset = b; }
};
END

