#pragma once
#include "physx/PxPhysicsAPI.h"

using namespace physx;

class CSimulationEventCallback :
    public PxSimulationEventCallback
{
public:
	CSimulationEventCallback();
	~CSimulationEventCallback() = default;

	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {};
	virtual void onWake(PxActor** actors, PxU32 count) {};
	virtual void onSleep(PxActor** actors, PxU32 count) {};
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {};
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count);
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};

};
