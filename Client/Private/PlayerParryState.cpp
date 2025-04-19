#include "Client_Pch.h"
#include "PlayerHurtState.h"
#include "PlayerParryState.h"
#include "PlayerIdleState.h"
#include "PlayerAttackState.h"
#include "PlayerWalkState.h"
#include "PlayerAvoidState.h"
#include "Player.h"
#include "Monster.h"
#include "MonsterState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Layer.h"
#include "Snow_Red.h"
#include "Effect.h"

CPlayerParryState::CPlayerParryState(ANIM_TYPE type)
{
    m_eType = type;
}

CPlayerParryState::~CPlayerParryState()
{
}

CPlayerState* CPlayerParryState::Transition(CPlayer& _player)
{
    if(_player.Get_HitDesc().bHit)
    {
        if (m_fTrackPos < 10.f || m_fTrackPos >= 15.f)
            return new CPlayerHurtState();
        else
        {
            _player.Get_HitDesc().bHit = false;
            _player.Get_TargetMonster()->Get_MonsterState()->Set_Attack(false);


            if (!m_bExecuted[0])
            {
                m_bExecuted[0] = true;
                m_pGameInstance->Play_Sound(L"BladeHit_01.wav", SOUND_PLAYER_ATTACK, 0.7f);
            }

            EFFECTDESC desc;
            desc.texName = TEXT("T_Flare_04");
            desc.vScale = { 5.f,5.f,5.f };
            desc.vPos = { 0,0,0,1 };
            desc.iRow = 1;
            desc.fTickTime = 1.f;
            desc.iShaderNum = 2;
            CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
                TEXT("Prototype_GameObject_Effect_Texture"), &desc);

            _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*0.5f + XMVectorSet(0,1,0,0);

            CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
            CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
            static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPos);

            CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
            wstring wstr = TEXT("../../SaveFiles/Particles/");
            wstr += TEXT("spark");
            wstr += TEXT(".effect");
            ifstream is(wstr, ios::binary);
            is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
            is.close();
            pDesc.bNum300 = true;
            XMStoreFloat4(&pDesc.vPivotPos, vPos);
            CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
                TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);
        }
    }
    else
    {
        if (!m_bExecuted[1])
        {
            m_bExecuted[1] = true;
            m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_PLAYER_SWING, 1.f);
        }
    }

    if (m_fTrackPos > m_fDuration - 100.f && m_pGameInstance->Keyboard_Down(VK_SPACE))
    {
        if (m_pGameInstance->Keyboard_Pressing('W'))
        {
            return new CPlayerAvoidState(AVOID_F);
        }
        if (m_pGameInstance->Keyboard_Pressing('A'))
        {
            return new CPlayerAvoidState(AVOID_L);
        }
        if (m_pGameInstance->Keyboard_Pressing('S'))
        {
            return new CPlayerAvoidState(AVOID_B);
        }
        if (m_pGameInstance->Keyboard_Pressing('D'))
        {
            return new CPlayerAvoidState(AVOID_R);
        }
    }

    if (m_fTrackPos > m_fDuration - 130.f && m_pGameInstance->Keyboard_Down('Q') && m_eType == PARRY1)
        return new CPlayerParryState(PARRY2);
    if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
        return new CPlayerIdleState();
    return nullptr;
}

void CPlayerParryState::Entry(CPlayer& _player)
{
    __super::Entry(_player);

    m_pModelCom->SetUp_Animation(m_eType, false);
}

void CPlayerParryState::Update(CPlayer& _player, _float fTimeDelta)
{
    XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
    m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

    if (m_fTrackPos > m_fDuration - 50.f && !m_bInterpolate)
    {
        m_bInterpolate = true;
        m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
        m_pModelCom->Set_NextAnim(IDLE);
    }

    __super::Update(_player, fTimeDelta);
    __super::RootPosUpdate(_player, 1.f, fTimeDelta);
    __super::PhysicsUpdate(_player, fTimeDelta);
}

void CPlayerParryState::Exit(CPlayer& _player)
{
    __super::Exit(_player);
    _player.Get_HitDesc().bHit = false;
}

void CPlayerParryState::Free()
{
}
