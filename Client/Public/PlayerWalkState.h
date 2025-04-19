#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerWalkState :
    public CPlayerState
{
public:
	CPlayerWalkState();
	virtual ~CPlayerWalkState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

private:
	_int m_iPrevIndex = -1;
	_vector m_vDir = { 0,0,0 };
	_bool m_bDirChange = false;
	_vector m_vModifiedDir = { 0,0,0 };
	_vector m_vPreDir = { 0,0,0 };
	_vector m_vMidDir = { 0,0,0 };
	_vector m_vCurDir = { 0,0,0 };
	_int m_iPrevDir = -1;
	_int m_iCurDir = -1;
	_bool m_bSpin = false;
	_bool m_bLockSpin = false;
	_float m_RotTime = 0.f;
	_float m_RotEndTime = 0.f;
	_float m_LockSpinTime = 0.f;
	_uint m_iLockSpinIndex = 0;
	_bool m_LockSpinMid = false;
	_float m_fStepSoundTime = -1.f;
};
END
