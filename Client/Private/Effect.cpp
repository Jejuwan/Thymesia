#include "Client_Pch.h"
#include "..\Public\Effect.h"
#include "GameInstance.h"
#include "Effect.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject(pDevice,pContext)
{
	
}

CEffect::CEffect(const CEffect & rhs)
	: CBlendObject(rhs)
{

}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	/* 백그라운드가 사용하기위한 컴포넌트들을 추가하낟. */

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(rand() % 20, 3.f, rand() % 20));

	//m_pTransformCom->Rotation({ 0,1,0 }, XMConvertToRadians(180.f));
	
	EFFECTDESC* pDesc = (EFFECTDESC*)pArg;
	if (nullptr != pDesc)
	{
		m_EffectDesc.texName = pDesc->texName;
		m_EffectDesc.meshPath = pDesc->meshPath;
		m_EffectDesc.vScale = pDesc->vScale;
		m_EffectDesc.vPos = pDesc->vPos;
		m_EffectDesc.iRow = pDesc->iRow;
		m_EffectDesc.iMaxFrame = pDesc->iRow * pDesc->iRow;
		if (pDesc->fTickTime < 0)
			m_EffectDesc.fTickTime = 90.f;
		else
			m_EffectDesc.fTickTime = pDesc->fTickTime;
			m_EffectDesc.iShaderNum = pDesc->iShaderNum;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaled({ XMVectorGetX(pDesc->vScale),XMVectorGetY(pDesc->vScale),XMVectorGetZ(pDesc->vScale) });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos);

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	

	m_fFrame += m_EffectDesc.fTickTime *fTimeDelta;

	if (m_fFrame < 1.f)
	{	

		m_iFrame = 0;
		if (m_EffectDesc.texName == L"T_Flare_04")
		{
			if (m_fFrame < 0.1f)
			{
				m_fAlpha += fTimeDelta * 5.f;
			}
			else if (m_fFrame > 0.1f)
			{
				m_fAlpha -= fTimeDelta * 5.f;
			}
			m_pTransformCom->Scaled(_float3(m_fFrame * 25, m_fFrame * 35, 1.f));

			m_fAlpha = max(0, min(0.9f, m_fAlpha));
			m_pTransformCom->LookAt({ pGameInstance->Get_CamPosition().x,
			pGameInstance->Get_CamPosition().y,pGameInstance->Get_CamPosition().z,1 });
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
		}
		else if(m_EffectDesc.meshPath != L"")
		{
			if (m_fFrame < 0.3f)
			{
				m_fAlpha += fTimeDelta * 6.f;
			}
			else if (m_fFrame > 0.3f)
			{
				m_fAlpha -= fTimeDelta * 6.f;
			}
			m_fAlpha = max(0, min(1.f, m_fAlpha));
		}
		else
		{
			m_pTransformCom->LookAt({ pGameInstance->Get_CamPosition().x,
			pGameInstance->Get_CamPosition().y,pGameInstance->Get_CamPosition().z,1 });
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
		}
	}
	else
	{
		m_iFrame = (_int)m_fFrame;
		m_pTransformCom->LookAt({ pGameInstance->Get_CamPosition().x,
			pGameInstance->Get_CamPosition().y,pGameInstance->Get_CamPosition().z,1 });
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
	}

	if (m_fFrame >= m_EffectDesc.iMaxFrame)
		m_bDead = true;

	if(m_TexcoordV<1.f)
	m_TexcoordV += 5.f*fTimeDelta;
	// SetUp_OnTerrain();
}

void CEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Compute_CamDistance(m_pTransformCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);	
}


HRESULT CEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;



	if (m_EffectDesc.meshPath == L"")
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(m_EffectDesc.iShaderNum);

		m_pVIBufferCom->Render();
	}
	else
	{
		if (FAILED(Bind_MeshShaderResources()))
			return E_FAIL;

		for (_uint i = 0; i < m_pModelCom->Get_NumMaterials(); i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pMeshShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pMeshShaderCom->Begin(1);

			m_pModelCom->Render(i);
		}
	}


		
	return S_OK;
}

HRESULT CEffect::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
			TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;
	
	/* For.Com_Transform */
	/* 각 객체들이 트랜스폼을 복제하여 객체안에 보관할 때! 객체가 움직, 회전해야한ㄴ 속도를 저장할 것이다. */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_MeshShader"), (CComponent**)&m_pMeshShaderCom)))
		return E_FAIL;

	wstring texName = TEXT("Prototype_Component_Texture_") + m_EffectDesc.texName;
	   
	   
	if (m_EffectDesc.meshPath == L"")
	{
		/* Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, texName,
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	if(m_EffectDesc.meshPath !=L"")
	{
		/* Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_EffectDesc.meshPath,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_ShaderResource_RTV(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", (_uint)0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrame", &m_iFrame, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iRow", &m_EffectDesc.iRow , sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;   
}

HRESULT CEffect::Bind_MeshShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pMeshShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(m_pMeshShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pMeshShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pMeshShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pMeshShaderCom->Bind_RawValue("g_TexcoordV", &m_TexcoordV, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect*	pInstance = new CEffect(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CEffect");
	}

	return pInstance;
}

CGameObject * CEffect::Clone(void* pArg)
{
	CEffect*	pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CEffect");
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMeshShaderCom);
}
