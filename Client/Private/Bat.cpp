#include "Client_Pch.h"
#include "Bat.h"
#include "GameInstance.h"
#include "BatIntroState.h"
#include "BatIdleState.h"
#include "Bone.h"
#include "Animation.h"


CBat::CBat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBat::CBat(const CBat& rhs)
	:CMonster(rhs)
{
}

HRESULT CBat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBat::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) 
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	PxCapsuleControllerDesc cDesc;
	cDesc.material = pGameInstance->GetMaterial();
	cDesc.position = { 0.f,0.f,55.f };
	cDesc.height = 2.0;
	cDesc.radius = 4.0;
	cDesc.slopeLimit = 0.0f;
	cDesc.contactOffset = 0.1f;
	cDesc.stepOffset = 0.02f;
	cDesc.reportCallback = this;

	m_pController = CGameInstance::Get_Instance()->CreateController(&cDesc);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0,5,-20.f,1 });

	_float3 vInitPos;
	XMStoreFloat3(&vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pController->setPosition({ vInitPos.x, vInitPos.y, vInitPos.z });

	m_CurState = new CBatIntroState;
	//m_CurState = new CBatIdleState;
	static_cast<CBatState*>(m_CurState)->Set_Bat(this);
	m_CurState->Entry(*this);

	m_eMonsterType = B_BAT;

	//¿ÞÆÈ
	PxShape* pAttackShape = pGameInstance->GetPhysics()->createShape(
		PxCapsuleGeometry((PxReal)1.5, (PxReal)2), *pGameInstance->GetMaterial());

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

	
	//¿À¸¥ÆÈ
	PxShape* pAttackShape2 = pGameInstance->GetPhysics()->createShape(
		PxCapsuleGeometry((PxReal)1.5, (PxReal)2), *pGameInstance->GetMaterial());

	pAttackShape2->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pAttackShape2->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	ATTACKDESC* adesc2 = new ATTACKDESC;
	adesc2->pShape = pAttackShape2;

	PxRigidStatic* pAttackActor2 = PxCreateStatic(*pGameInstance->GetPhysics(), localTm, *pAttackShape2);
	adesc2->pActor = pAttackActor2;
	adesc2->bActive = true;

	m_pAttackDesc.push_back(adesc2);

	pGameInstance->GetScene()->addActor(*pAttackActor2);


	//¸Ó¸®
	PxShape* pAttackShape3 = pGameInstance->GetPhysics()->createShape(
		PxSphereGeometry((PxReal)3), *pGameInstance->GetMaterial());

	pAttackShape3->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pAttackShape3->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	ATTACKDESC* adesc3 = new ATTACKDESC;
	adesc3->pShape = pAttackShape3;

	PxRigidStatic* pAttackActor3 = PxCreateStatic(*pGameInstance->GetPhysics(), localTm, *pAttackShape3);
	adesc3->pActor = pAttackActor3;
	adesc3->bActive = true;

	m_pAttackDesc.push_back(adesc3);

	pGameInstance->GetScene()->addActor(*pAttackActor3);


	m_pModelCom->Set_Animation_Speed(4.f);
	m_pModelCom->Get_Anim(0)->Set_Speed(0.001f);
	m_pModelCom->Get_Anim(11)->Set_Speed(2.f);
	m_pModelCom->Get_Anim(14)->Set_Speed(2.f);
	m_pModelCom->Get_Anim(15)->Set_Speed(2.f);
	m_pModelCom->Get_Anim(10)->Set_Speed(6.f);

	m_iHead_BoneIdx = m_pModelCom->Get_BoneIndex("head");
	m_iArmL_BoneIdx = m_pModelCom->Get_BoneIndex("hand_l");
	m_iArmR_BoneIdx = m_pModelCom->Get_BoneIndex("hand_r");

	m_pModelCom->SetPivotMatrix(XMLoadFloat4x4(&m_pModelCom->GetPivotMatrix()) * XMMatrixTranslation(0, 0, -1));
	
	Safe_Release(pGameInstance);

	return S_OK;
}

void CBat::Tick(_float fTimeDelta)
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


	_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();

	//¿ÞÆÈ

	_float4x4 weaponMat;
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[m_iArmL_BoneIdx]->Get_CombinedTransformationMatrix());
	_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) * XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	_float4x4 transformMat;
	XMStoreFloat4x4(&transformMat, finalMat);

	_float3 vWeaponPos;
	XMStoreFloat3(&vWeaponPos,{transformMat._41,transformMat._42,transformMat._43});

	_float4 vWeaponQuat;
	XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&transformMat)));
	m_pAttackDesc[0]->pActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
		PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));

	//¿À¸¥ÆÈ 
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[m_iArmR_BoneIdx]->Get_CombinedTransformationMatrix());
	finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) *XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	XMStoreFloat4x4(&transformMat, finalMat);

	XMStoreFloat3(&vWeaponPos, { transformMat._41,transformMat._42,transformMat._43 });

	XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&transformMat)));
	m_pAttackDesc[1]->pActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
		PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));

	//¸Ó¸®
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[m_iHead_BoneIdx]->Get_CombinedTransformationMatrix());
	finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) * XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	XMStoreFloat4x4(&transformMat, finalMat);

	XMStoreFloat3(&vWeaponPos, { transformMat._41,transformMat._42,transformMat._43 });

	XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&transformMat)));
	m_pAttackDesc[2]->pActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
		PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));

	XMStoreFloat4(&CGameInstance::Get_Instance()->Get_LightDesc(2)->vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0,2,0,0));
}

void CBat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBat::Add_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_WeaponR_Transform"), (CComponent**)&m_pWeaponRTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("B_Bat_Anim"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CBat* CBat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBat* pInstance = new CBat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : Villager_M");
	}

	return pInstance;
}

CGameObject* CBat::Clone(void* pArg)
{
	CBat* pInstance = new CBat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : Villager_M");
	}

	return pInstance;
}

void CBat::Free()
{
	__super::Free();
}

bool CBat::filter(const PxController& a, const PxController& b)
{
	int fdsaf = 4;
	return false;
}
