#include "Client_Pch.h"
#include "Bat_BoomBall.h"
#include "GameInstance.h"
#include "Player.h"

CBat_BoomBall::CBat_BoomBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster(pDevice,pContext)
{
}

CBat_BoomBall::CBat_BoomBall(const CBat_BoomBall& rhs)
    :CMonster(rhs)
{
}

HRESULT CBat_BoomBall::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBat_BoomBall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();

    PxShape* pAttackShape = pGameInstance->GetPhysics()->createShape(
        PxSphereGeometry((PxReal)1), *pGameInstance->GetMaterial());

    pAttackShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    pAttackShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    ATTACKDESC* adesc = new ATTACKDESC;
    adesc->pShape = pAttackShape;

    PxTransform localTm(0, 0, 0);
    PxRigidStatic* pAttackActor = PxCreateStatic(*pGameInstance->GetPhysics(), localTm, *pAttackShape);
    adesc->pActor = pAttackActor;
    adesc->bActive = true;
    adesc->bHit = false;

    m_pAttackDesc.push_back(adesc);

    pGameInstance->GetScene()->addActor(*pAttackActor);

    m_pTransformCom->Scaled({ 10.f,10.f,10.f });

    m_fRimStrength = 0.f;

    return S_OK;
}

void CBat_BoomBall::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();

    m_pTransformCom->Go_Straight(fTimeDelta * 10.f);
    _float3 vPos;
    XMStoreFloat3(&vPos,m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    m_pAttackDesc[0]->pActor->setGlobalPose(PxTransform(PxVec3(vPos.x, vPos.y, vPos.z)));
    if (m_pAttackDesc[0]->bHit)
    {
        static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_HitDesc().bHit = true;
        static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_HitDesc().eType = HIT_TYPE::STRONG;
        m_pAttackDesc[0]->bHit = false;
    }
}

void CBat_BoomBall::Late_Tick(_float fTimeDelta)
{ 
    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CBat_BoomBall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

   // m_pShaderCom->Begin(0);

    for (_uint i = 0; i < m_pModelCom->Get_NumMaterials(); i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pNonAnimShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;


        /*if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;*/

        m_pNonAnimShaderCom->Begin(3);

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CBat_BoomBall::Add_Components()
{
    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Bat_Boomball"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_AlphaShader"), (CComponent**)&m_pAlphaShaderCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBat_BoomBall::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pNonAnimShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();

    if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fAlpha = 0.5f;
    if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_fAlpha", &fAlpha,sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimStrength, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
        return E_FAIL;
    return S_OK;
}

CBat_BoomBall* CBat_BoomBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBat_BoomBall* pInstance = new CBat_BoomBall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Created : Bat_Boomball");
    }

    return pInstance;
}

CGameObject* CBat_BoomBall::Clone(void* pArg)
{
    CBat_BoomBall* pInstance = new CBat_BoomBall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Cloned : Bat_Boomball");
    }

    return pInstance;
}

void CBat_BoomBall::Free()
{
    __super::Free();

    Safe_Release(m_pAlphaShaderCom);
}
