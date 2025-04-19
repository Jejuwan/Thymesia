#include "Client_Pch.h"
#include "MonsterDeadState.h"
#include "MonsterStunState.h"
#include "MonsterIdleState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerAttackState.h"
#include "UI.h"
#include "Camera_Player.h"

CMonsterStunState::CMonsterStunState()
{
}

CMonsterStunState::~CMonsterStunState()
{
}

CMonsterState* CMonsterStunState::Transition(CMonster& _monster)
{
    if (_monster.Get_MonsterHit() && m_pPlayer->Get_PlayerState()->Get_Attack() &&
        ((m_pModelCom->Get_CurrentAnimIndex()==STUNSTART && m_fTrackPos > 40.f) || m_pModelCom->Get_CurrentAnimIndex()==STUNLOOP))
        return new CMonsterDeadState();
    return nullptr;
}

void CMonsterStunState::Entry(CMonster& _monster)
{
    __super::Entry(_monster);

    m_szName = "Stun";
    m_pModelCom->SetUp_Animation(STUNSTART, false);

    CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() + 1));
    pUI->Get_Desc()->bVisible = true;

    m_bHitRim = true;

    CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
    wstring wstr = TEXT("../../SaveFiles/Particles/");
    wstr += TEXT("blood");
    wstr += TEXT(".effect");
    ifstream is(wstr, ios::binary);
    is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
    is.close();
    pDesc.bNum300 = true;
    _vector vPos;
    _vector vRight = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    _vector vLook = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    switch (m_pPlayer->Get_PlayerState()->Get_AttackIndex())
    {
    case 0:
        XMStoreFloat3(&pDesc.fMinDir, (-vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        XMStoreFloat3(&pDesc.fMaxDir, (-vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.5, 0, 0);
        break;
    case 1:
        XMStoreFloat3(&pDesc.fMinDir, (vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        XMStoreFloat3(&pDesc.fMaxDir, (vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
        break;
    case 2:
        XMStoreFloat3(&pDesc.fMinDir, (vLook - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        XMStoreFloat3(&pDesc.fMaxDir, (vLook + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
        vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
        break;
    default:
        XMStoreFloat3(&pDesc.fMinDir, (-vLook - XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
        XMStoreFloat3(&pDesc.fMaxDir, (-vLook + XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
        vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
        break;
    }
    vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
    XMStoreFloat4(&pDesc.vPivotPos, vPos);
    CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
        TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);

    wstring soundName = L"BodyStab_0";
    soundName += to_wstring(2);
    soundName += L".wav";
    switch (_monster.Get_HpBarIdx())
    {
    case 5:
        m_pGameInstance->StopSound(SOUND_ENEMY1);
        m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY1, 1.f);
        break;
    case 8:
        m_pGameInstance->StopSound(SOUND_ENEMY2);
        m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY2, 1.f);
        break;
    case 11:
        m_pGameInstance->StopSound(SOUND_ENEMY3);
        m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY3, 1.f);
        break;
    }

   
        m_pGameInstance->StopSound(SOUND_ENEMY_HIT);
        m_pGameInstance->Play_Sound(L"BossBat_StunStart.wav", SOUND_ENEMY_HIT, 0.6f);
  }

void CMonsterStunState::Update(CMonster& _monster, _float fTimeDelta)
{
    XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();
    m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();

    if (m_pModelCom->Get_CurrentAnimIndex() == STUNSTART)
    {
        if(m_fTrackPos < 50.f)
        {
            _float fSpeed = 0;
            switch (_monster.Get_MonsterType())
            {
            case CMonster::GARDENER:
                fSpeed = 0.03f;
                break;
            case CMonster::JOKER:
                fSpeed = 0.015f;
                break;
            case CMonster::VILLAGER_M:
                fSpeed = 0.1f;
                break;
            case CMonster::VILLAGER_F:
                fSpeed = 0.0f;
                break;
            }
            m_pTransformCom->Go_Dir({ 0,-1,0 }, fSpeed, fTimeDelta);
        }
        _float fCurPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        //rootpos update
        if (!m_bControllerHit && !m_bSideColided)
        {
            _vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
            _vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
            _matrix m = XMMatrixRotationQuaternion(v);

            _float4 vPos;
            XMStoreFloat4(&vPos, XMLoadFloat4(&m_vInitPos) - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()), m) * 0.5f * 0.016f);
            _float4 vPosition = { vPos.x, fCurPosY,vPos.z,1.f };
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
        }

        if (m_pModelCom->Get_CurrentAnim()->GetEnd())
        {
            m_pModelCom->Get_CurrentAnim()->ResetEnd();
            m_pModelCom->SetUp_Animation(STUNLOOP, true);
            XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        }
    }
    __super::Update(_monster, fTimeDelta);
    
    //physical update
    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float3 vDisp;
    XMStoreFloat3(&vDisp, vPos - XMLoadFloat3(&m_vPrePos));
    m_CollisionFlag = _monster.Get_Controller()->move(PxVec3(vDisp.x, vDisp.y, vDisp.z), 0.0f, fTimeDelta, PxControllerFilters(), CGameInstance::Get_Instance()->GetObstacleContext());
    if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        m_fTime = 0.f;
    }
    else if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_SIDES)
    {
        m_bSideColided = true;
    }
    PxExtendedVec3 vControllerPos = _monster.Get_Controller()->getFootPosition();

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4((float)vControllerPos.x, (float)XMVectorGetY(vPos), (float)vControllerPos.z, 1.f)));
}
   

void CMonsterStunState::Exit(CMonster & _monster)
{
	__super::Exit(_monster);
    CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() + 1));
    pUI->Get_Desc()->bVisible = false;
}

void CMonsterStunState::Free()
{
}
