#pragma once
#include "PlayerState.h"

BEGIN(Client)
class CPlayerExecuteState :
	public CPlayerState
{
public:
	CPlayerExecuteState();
	virtual ~CPlayerExecuteState();

	virtual CPlayerState* Transition(CPlayer& _player);
	virtual void Entry(CPlayer& _player);
	virtual void Update(CPlayer& _player, _float fTimeDelta);
	virtual void Exit(CPlayer& _player);
	virtual void Free() override;

private:
	_vector m_vPreLook;
	_vector m_vNextLook;
	_vector m_vPreCamPos;
	_float fCameraMoveTime = 0.f;
	_bool m_bIntroAnim = { false };
	_bool m_bExecuteEnd = { false };
};
END
