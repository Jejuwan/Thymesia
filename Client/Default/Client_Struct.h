#pragma once
#include "Client_Pch.h"
#include "Client_Defines.h"
#include "Engine_Defines.h"

enum HIT_TYPE {DEFAULT, STRONG, FALLDOWN, HEADACHE};
enum UI_TYPE{ORTHOGRAPHIC,PERSPECTIVE};

BEGIN(Client)
typedef struct tagSpawnDesc
{
	LEVEL m_eCurLevel;
}SPAWNDESC;

typedef struct tagAttackActorDesc
{
	PxShape* pShape;
	PxRigidStatic* pActor;
	_bool bActive;
	_bool bHit;
}ATTACKDESC;

typedef struct tagHitDesc
{
	_bool bHit;
	HIT_TYPE eType;
}HITDESC;

typedef struct tagEffectDesc
{
	wstring texName;
	wstring meshPath = L"";
	_vector vScale;
	_vector vPos;
	_int    iRow;
	_int	iMaxFrame;
	_float  fTickTime = -1.f;
	_int	iShaderNum = 1;
}EFFECTDESC;

typedef struct tagUIDesc
{
	wstring texName;
	UI_TYPE eType = ORTHOGRAPHIC;
	_bool bVisible = true;
	_bool bLookAt = false;
	_float2 vScale;
	_float3 vPos;
	_float fAngle;
}UIDESC;
END