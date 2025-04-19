#include "Client_Pch.h"
#include "Gardener.h"
#include "GameInstance.h"
#include "MonsterIdleState.h"
#include "Bone.h"
#include "Animation.h"


CGardener::CGardener(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CGardener::CGardener(const CGardener& rhs)
	:CMonster(rhs)
{
}

HRESULT CGardener::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGardener::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	PxCapsuleControllerDesc cDesc;
	cDesc.material = pGameInstance->GetMaterial();
	cDesc.position = { 0.f,0.f,0.f };
	cDesc.height = 2.0;
	cDesc.radius = 0.5;
	cDesc.slopeLimit = 0.0f;
	cDesc.contactOffset = 0.1f;
	cDesc.stepOffset = 0.02f;
	cDesc.reportCallback = this;

	m_pController = CGameInstance::Get_Instance()->CreateController(&cDesc);

	_float3 vInitPos;
	XMStoreFloat3(&vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pController->setPosition({ vInitPos.x, vInitPos.y, vInitPos.z });

	m_CurState = new CMonsterIdleState;
	m_CurState->Entry(*this);
	m_eMonsterType = GARDENER;


	PxShape* pAttackShape = pGameInstance->GetPhysics()->createShape(
		PxCapsuleGeometry((PxReal).3, (PxReal)1.2), *pGameInstance->GetMaterial());

	pAttackShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pAttackShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	ATTACKDESC* adesc = new ATTACKDESC;
	adesc->pShape = pAttackShape;

	PxTransform localTm(0, 0, 0);
	PxRigidStatic* pAttackActor = PxCreateStatic(*pGameInstance->GetPhysics(), localTm, *pAttackShape);
	adesc->pActor = pAttackActor;
	adesc->bActive = true;

	m_pAttackDesc.push_back(adesc);

	pGameInstance->GetScene()->addActor(*pAttackActor);

	m_pModelCom->Set_Animation_Speed(2.f);
	m_pModelCom->Get_Anim(6)->Set_Speed(4.f);
	m_pModelCom->Get_Anim(7)->Set_Speed(4.f);

	m_iWeaponR_BoneIdx = m_pModelCom->Get_BoneIndex("weapon_r");

	return S_OK;
}

void CGardener::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CMonsterState* pState = m_CurState->Transition(*this);
	if (nullptr != pState)
	{
		m_CurState->Exit(*this);
		Safe_Release(m_CurState);
		m_CurState = pState;
		m_CurState->Entry(*this);
	}
	m_CurState->Update(*this, fTimeDelta);

	_float3 vWeaponPos;
	XMStoreFloat3(&vWeaponPos, m_pWeaponRTransformCom->Get_State(CTransform::STATE_POSITION));

	_float4 vWeaponQuat;

	XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pWeaponRTransformCom->Get_WorldMatrix())));

	_matrix rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&vWeaponQuat));
	//vWeaponPos.x += rotMat.r[1].m128_f32[0];
	//vWeaponPos.y += rotMat.r[1].m128_f32[1];
	//vWeaponPos.z += rotMat.r[1].m128_f32[2];

	m_pAttackDesc[0]->pActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
		PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));

}

void CGardener::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CGardener::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGardener::Add_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_WeaponR_Transform"), (CComponent**)&m_pWeaponRTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Gardener_Anim"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Gardener_Scythe"),
		TEXT("Com_WeaponR_Model"), (CComponent**)&m_pWeaponRModelCom)))
		return E_FAIL;

	return S_OK;
}

CGardener* CGardener::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGardener* pInstance = new CGardener(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CGardener");
	}

	return pInstance;
}

CGameObject* CGardener::Clone(void* pArg)
{
	CGardener* pInstance = new CGardener(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CGardener");
	}

	return pInstance;
}

void CGardener::Free()
{
	__super::Free();
}
