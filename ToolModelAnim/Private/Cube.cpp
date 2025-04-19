#include "ToolModelAnim_Pch.h"
#include "..\Public\Cube.h"
#include "GameInstance.h"
#include "MapToolUI.h"


CCube::CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCube::CCube(const CCube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCube::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCube::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

 	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pModelCom->SetUp_Animation(rand() % 15);
	return S_OK;
}

void CCube::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//m_pModelCom->Play_Animation(fTimeDelta);

	/*PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	const PxU32 nbShapes = m_pRigidBody->getNbShapes();
	PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
	m_pRigidBody->getShapes(shapes, nbShapes);

	if(m_bEnablePhysics)
	{
		
		for (PxU32 j = 0; j < nbShapes; j++) {
			
			const PxMat44 shapePose(
				PxShapeExt::getGlobalPose(*shapes[j], *m_pRigidBody));

			if (m_pRigidBody->is<PxRigidDynamic>()) {

				bool speeping = m_pRigidBody->is<PxRigidDynamic>() &&
					m_pRigidBody->is<PxRigidDynamic>()->isSleeping();

				CTransform* transform = static_cast<CTransform*>(this->Get_Component(TEXT("Com_Transform")));
				if (transform != nullptr)
					transform->Set_WorldMatrix(SimpleMath::Matrix(shapePose.front()) * SimpleMath::Matrix::CreateScale(1.00f));
			}
		}
	}
	else
	{
		m_pRigidBody->putToSleep();
	}
	XMStoreFloat3(&(Get_Bounding_Box()->Center), m_pTransformCom->Get_State(CTransform::STATE_POSITION));*/

}

void CCube::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CCube::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	for (_uint i = 0; i < m_pModelCom->Get_NumMaterials(); i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

	/*	if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;*/

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCube::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona4"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Default"),
	//	TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CCube::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CCube* CCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCube* pInstance = new CCube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCube");
	}

	return pInstance;
}

CGameObject* CCube::Clone(void* pArg)
{
	CCube* pInstance = new CCube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCube");
	}

	return pInstance;
}

void CCube::Cook()
{
	_float4x4 mat = m_pTransformCom->Get_WorldMatrix();
	PxTransform pose = PxTransform(PxIdentity);
	pose.p = { mat._41,mat._42,mat._43 };
	_float4 vQuat;
	XMStoreFloat4(&vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&mat)));
	pose.q = { vQuat.x,vQuat.y,vQuat.z,vQuat.w };

	_float3 vScale = m_pTransformCom->Get_Scaled();

	size_t size = m_pModelCom->Get_TriangleMeshes().size();
	for(int i=0;i<size;i++)
	{
		PxRigidStatic* hfActor = CGameInstance::Get_Instance()->GetPhysics()->createRigidStatic(pose);
		PxTriangleMeshGeometry hfGeom(m_pModelCom->Get_TriangleMeshes()[i], PxMeshScale({ vScale.x, vScale.y, vScale.z }));
		PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*hfActor, hfGeom, *CGameInstance::Get_Instance()->GetMaterial());
		CGameInstance::Get_Instance()->GetScene()->addActor(*hfActor);
	}
}

void CCube::SetBoundingBox()
{
	pair<_float3, _float3> minMaxPoint = m_pModelCom->Get_BoundingBoxMinMaxPoint();
	_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	_vector minPoint = { minMaxPoint.first.x,minMaxPoint.first.y,minMaxPoint.first.z,1.f };
	_vector maxPoint = { minMaxPoint.second.x, minMaxPoint.second.y, minMaxPoint.second.z, 1.f };

	minPoint = XMVector4Transform(minPoint, worldMat);
	maxPoint = XMVector4Transform(maxPoint, worldMat);

	_float3 diff;
	XMStoreFloat3(&diff, maxPoint - minPoint);
	XMStoreFloat3(&Get_Bounding_Box()->Extents, {fabs(diff.x)/2.f,fabs(diff.y) / 2.f, fabs(diff.z) / 2.f});
	XMStoreFloat3(&Get_Bounding_Box()->Center, (maxPoint + minPoint) / 2.f);
	XMStoreFloat4(&Get_Bounding_Box()->Orientation, XMQuaternionRotationMatrix(worldMat));
}


void CCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
