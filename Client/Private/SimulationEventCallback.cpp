#include "Client_Pch.h"
#include "..\Public\SimulationEventCallback.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Monster.h"
#include "Player.h"
#include "PlayerState.h"
#include "MonsterState.h"
#include "Transform.h"
#include "Bat_BoomBall.h"

CSimulationEventCallback::CSimulationEventCallback()
{
}

void CSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	 //ignore pairs when shapes have been deleted
	if (pairs->flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
		PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Get_Player());
	CLayer* pMonsterLayer = pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	if (nullptr == pMonsterLayer)
		return;
	int size = pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"))->size();

	for(int i=0;i<count;i++)
	{
		const PxTriggerPair& pair = pairs[i];
		for (int j = 0; j < size; j++)
		{
			CMonster* pMonster = static_cast<CMonster*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), j));

			//플레이어 공격 액터가 몬스터와 접촉중일 때 
			if (pair.triggerActor == pPlayer->Get_AttackActor()
				&& nullptr != pMonster->Get_Controller()
				&& pair.otherActor == pMonster->Get_Controller()->getActor())
			{
				pMonster->Set_MonsterHit(true);
			}

			//플레이어 공격액터가 몬스터를 벗어났을 떄
			if (pair.triggerActor == pPlayer->Get_AttackActor()
				&& nullptr != pMonster->Get_Controller()
				&& pair.otherActor == pMonster->Get_Controller()->getActor()
				&& pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				pMonster->Set_MonsterHit(false);
			}

			_int iActorSize = pMonster->Get_AttackDesc().size();
			for(int k=0; k< iActorSize; k++)
			{
				//몬스터 공격액터가 플레이어와 접촉중일 때
				if (pair.triggerActor == pMonster->Get_AttackDesc()[k]->pActor
					&& pair.otherActor == pPlayer->Get_Controller()->getActor())
				{
					pMonster->Get_AttackDesc()[k]->bHit = true;
					if(nullptr==dynamic_cast<CBat_BoomBall*>(pMonster))
						pPlayer->Set_TargetMonster(pMonster);
				}

				//몬스터 공격액터가 플레이어를 벗어났을 때 
				if (pair.triggerActor == pMonster->Get_AttackDesc()[k]->pActor
					&& pair.otherActor == pPlayer->Get_Controller()->getActor()
					&& pair.status == PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					pMonster->Get_AttackDesc()[k]->bHit = false;
				}
			}
		}
	}
}
