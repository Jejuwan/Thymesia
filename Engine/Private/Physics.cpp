#include "Engine_Pch.h"
#include "Physics.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPhysics)

CPhysics::CPhysics()
{
}

CPhysics::~CPhysics()
{
}


void CPhysics::createStack(CTransform* transform, physx::PxRigidDynamic** rigid)
{
    PxShape* shape = gPhysics->createShape(
        PxBoxGeometry(transform->Get_Scaled().x, transform->Get_Scaled().y, transform->Get_Scaled().z), *gMaterial);

    _vector vPos = transform->Get_State(CTransform::STATE_POSITION);
    PxTransform localTm(XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos));
    *rigid =
        gPhysics->createRigidDynamic(localTm);
    (*rigid)->attachShape(*shape);
    // body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

    PxRigidBodyExt::updateMassAndInertia(**rigid, 10.0f);
    gScene->addActor(**rigid);
    shape->release();
}

void CPhysics::InitPhysics(bool interactive)
{
    gFoundation =
        PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport =
        PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
        PxTolerancesScale(), true, gPvd);

    //gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f * 5.f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
            true);
    }
    gMaterial = gPhysics->createMaterial(0.2f, 0.2f, 0.3f);

    gManager = PxCreateControllerManager(*gScene);
  /*  gManager->setDebugRenderingFlags(PxControllerDebugRenderFlag::eALL);
    PxRenderBuffer& renderBuffer = mControllerManager->getRenderBuffer();
    RenderPhysX3Debug* renderer = getDebugRenderer();
    renderer->update(renderBuffer);
    renderBuffer.clear();*/

}

void CPhysics::Tick()
{
    gScene->simulate(1.0f / 60.0f);
    gScene->fetchResults(true);


    // gScene->getActors()
    // PxGeometryType::eBOX: , case PxGeometryType::eSPHERE:

    //PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC |
    //    PxActorTypeFlag::eRIGID_STATIC);
    //std::vector<PxRigidActor*> actors(nbActors);
    //gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC |
    //    PxActorTypeFlag::eRIGID_STATIC,
    //    reinterpret_cast<PxActor**>(&actors[0]), nbActors);

    //PxShape* shapes[MAX_NUM_ACTOR_SHAPES];

    //int count = 0;

    //for (PxU32 i = 0; i < nbActors; i++) {

    //   
    //}
}

void CPhysics::Clear()
{
    std::vector<PxRigidActor*> actorsToDelete;
  
    gManager->purgeControllers();
    
    PxU32 numActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
    actorsToDelete.resize(numActors);
    gScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<PxActor**>(&actorsToDelete[0]), numActors);

    // 모든 액터를 제거합니다.
    for (PxU32 i = 0; i < numActors; ++i) {
        gScene->removeActor(*actorsToDelete[i]);
        actorsToDelete[i]->release(); // 액터의 메모리를 해제합니다.
    }
}

void CPhysics::Free()
{
    gManager->purgeControllers();

    PX_RELEASE(gManager)
    //PX_RELEASE(gCooking)
    PX_RELEASE(gMaterial)
    PX_RELEASE(gScene);
    PX_RELEASE(gDispatcher);
    PX_RELEASE(gPhysics);
    if (gPvd) {
        PxPvdTransport* transport = gPvd->getTransport();
        gPvd->release();
        gPvd = NULL;
        PX_RELEASE(transport);
    }
    PX_RELEASE(gFoundation);
}

PxController* CPhysics::CreateController(PxCapsuleControllerDesc* desc)
{
    return gManager->createController(*desc);
}
