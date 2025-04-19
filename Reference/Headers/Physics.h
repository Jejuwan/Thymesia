#pragma once
#include "Base.h"
#include "physx/PxPhysicsAPI.h"

using namespace physx;

#define PX_RELEASE(x)                                                          \
    if (x) {                                                                   \
        x->release();                                                          \
        x = NULL;                                                              \
    }

#define PVD_HOST "127.0.0.1"
#define MAX_NUM_ACTOR_SHAPES 128

BEGIN(Engine)
class ENGINE_DLL CPhysics :
    public CBase
{
    DECLARE_SINGLETON(CPhysics)
public:
    CPhysics();
    ~CPhysics();


    void createStack(class CTransform* transform, physx::PxRigidDynamic** rigid);

    void InitPhysics(bool interactive);
    void Tick();
    void Clear();

public:
    PxDefaultAllocator gAllocator;
    PxDefaultErrorCallback gErrorCallback;
    PxFoundation* gFoundation = NULL;
    PxPhysics* gPhysics = NULL;
    PxDefaultCpuDispatcher* gDispatcher = NULL;
    PxScene* gScene = NULL;
    PxMaterial* gMaterial = NULL;
    PxPvd* gPvd = NULL;
    PxReal stackZ = 10.0f;
    PxControllerManager* gManager = NULL;
    PxObstacleContext* gObstacleContext= NULL;
public:
    virtual void Free();

   PxPhysics* GetPhysics() { return gPhysics; }
   PxScene* GetScene() { return gScene; }
   PxMaterial* GetMaterial() { return gMaterial; }
   //PxCooking* GetCooking() { return gCooking; }
   PxObstacleContext* GetObstacleContext() { return gObstacleContext; }
   PxControllerManager* GetControllerManager() { return gManager; }

   PxController* CreateController(PxCapsuleControllerDesc* desc);
};
END
