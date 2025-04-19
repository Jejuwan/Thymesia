#include "Engine_Pch.h"
#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "BlendObject.h"
#include "Target_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_uint			iNumViewPorts(1);
	m_pContext->RSGetViewports(&iNumViewPorts, &m_OriginViewport);
	m_ShadowViewport = m_OriginViewport;
	m_ShadowViewport.Width *= 6;
	m_ShadowViewport.Height *= 6;

	/* Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Rim */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Rim"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	/* Target_ShadowDepth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), m_ShadowViewport.Width, m_ShadowViewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.0f, 0.0f, 1.f)/*,CRenderTarget::TYPE::DEPTH_STENCIL*/)))
		return E_FAIL;

	///* Target_ShadowFactor */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowFactor"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.0f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Post"), m_OriginViewport.Width, m_OriginViewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Rim"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ShadowFactor"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Post"), TEXT("Target_Post"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;


	XMStoreFloat4x4(&m_WorldMatrix_Deferred, XMMatrixScaling(m_OriginViewport.Width, m_OriginViewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix_Deferred, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix_Deferred, XMMatrixOrthographicLH(m_OriginViewport.Width, m_OriginViewport.Height, 0.f, 1.f));


#ifdef _DEBUG
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 300.0f, 100.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 500.0f, 100.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 100.0f, 300.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.0f, 300.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Rim"), 500.0f, 300.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Post"), 500.0f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 500.0f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowFactor"), 500.0f, 500.f, 200.0f, 200.0f)))
	//	return E_FAIL;

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderGroup[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	/* �޾ƿ� ����(��ǻ��, �븻)�� �� ������ �ϰ� ���̵� Ÿ�ٿ� �������. . */
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	/* ���۵�� ���� ��������� ������. */

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_Post()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	m_DebugComponents.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}
#endif

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{

	m_pContext->RSSetViewports(1, &m_ShadowViewport);

	/* Diffuse + Normal */
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow"));

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Shadow();

		//Safe_Release(pGameObject);
	}


	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow"));

	m_pContext->RSSetViewports(1, &m_OriginViewport);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Clear_DepthStencil_View();


	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"));

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONBLEND].clear();
	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_GameObjects"));

	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	/* Diffuse + Normal */
	//m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Shadow"));

	//for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	//{
	//	if (nullptr != pGameObject)
	//		pGameObject->Render();

	//	Safe_Release(pGameObject);
	//}

	//m_RenderGroup[RENDER_NONBLEND].clear();

	//m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Shadow"));

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade + Specular*/
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"));

	/* ���̴��� ��� Ÿ���� ���ε��Ѵ�. */
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	/* ���̴��� ���� Ÿ���� ���ε��Ѵ�. */
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	///* ����ŧ�� �ؽ��ĸ� ���̴��� ���ε� .*/
	//if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
	//	return E_FAIL;

	//if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Rim"), "g_RimTexture")))
	//	return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_ShadowDepth"), "g_ShadowTexture")))
		return E_FAIL;

	/* ���̴��� ������ ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	/* ���̴��� ���� ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	/* ���̴��� ������ ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	m_pLight_Manager->Render_Lights(m_pShader, m_pVIBuffer);

	/* ����۷� �����Ѵ�. */
	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"));

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	/* ����ۿ��ٰ� �������� �� �������� ����� �׷����� .*/

	/* ���̴��� ������ ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	/* ���̴��� ���� ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	/* ���̴��� ������ ���ε��Ѵ�. */
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	/* ��ǻ�� �ؽ��ĸ� ���̴��� ���ε� .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	/* ���̵� �ؽ��ĸ� ���̴��� ���ε� .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;

	/* ����ŧ�� �ؽ��ĸ� ���̴��� ���ε� .*/
	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Rim"), "g_RimTexture")))
		return E_FAIL;

	_float fFadeAlpha = CGameInstance::Get_Instance()->Get_FadeAlpha();
	if (FAILED(m_pShader->Bind_RawValue("g_fFadeAlpha", &fFadeAlpha, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONLIGHT].clear();

	return S_OK;
}

_bool Compare(CGameObject* pSour, CGameObject* pDest)
{
	return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
}

HRESULT CRenderer::Render_Blend()
{
	m_RenderGroup[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
		});

	// m_RenderGroup[RENDER_BLEND].sort(Compare);


	for (auto& pGameObject : m_RenderGroup[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_BLEND].clear();


	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Post()
{
	m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Post"));

	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Post"));

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_GameObjects"));
	m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_LightAcc"));
	//m_pTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_Shadow"));

	return S_OK;
}
#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CRenderer");
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}


}
