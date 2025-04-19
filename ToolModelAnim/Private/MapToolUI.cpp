#include "ToolModelAnim_Pch.h"
#include "MapTool_Defines.h"
#include "..\Public\MapToolUI.h"
#include "GameInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Layer.h"
#include "Cube.h"

IMPLEMENT_SINGLETON(CMapToolUI)


CMapToolUI::CMapToolUI()
{
}

void CMapToolUI::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize = ImVec2(g_iWinSizeX, g_iWinSizeY);
	ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGuizmo::SetOrthographic(false);
}

void CMapToolUI::Tick(_float fTimeDelta)
{
	UpdateUI();
}

void CMapToolUI::Render()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CMapToolUI::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


void CMapToolUI::UpdateUI()
{

	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();


	_float4x4 mat = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	float cameraView[16] =
	{
		mat._11,mat._12,mat._13,mat._14,
		mat._21,mat._22,mat._23,mat._24,
		mat._31,mat._32,mat._33,mat._34,
		mat._41,mat._42,mat._43,mat._44
	};

	_float4x4 mat2 = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	float cameraProjection[16] =
	{
		mat2._11,mat2._12,mat2._13,mat2._14,
		mat2._21,mat2._22,mat2._23,mat2._24,
		mat2._31,mat2._32,mat2._33,mat2._34,
		mat2._41,mat2._42,mat2._43,mat2._44
	};

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


	if (m_selectedObj != nullptr)
	{
		ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, m_mode, m_vObjMat, NULL, nullptr);

		_float4x4 mat;
		for (int i = 0; i < 4; i++)
			memcpy(&mat.m[i], &m_vObjMat[i * 4], sizeof(float) * 4);
		m_selectedObjTransform->Set_WorldMatrix(mat);
	}

 	ImGui::Begin("Scene Control");


	if (ImGui::CollapsingHeader("GameObject"))
	{
		if (ImGui::Button("Create"))
		{
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model"),
				TEXT("Prototype_GameObject_Cube"));
			m_selectedObj = CGameInstance::Get_Instance()->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model"), 0);
			m_selectedObjTransform = static_cast<CTransform*>(m_selectedObj->Get_Component(TEXT("Com_Transform")));
		/*	
				for (int i = 0; i < 3; i++)
				{
					VTXANIMMESH aaa;
					aaa.Deserialize(aaa);
					int a = 5;
				}*/
			
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			
		}
		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (m_selectedObj != nullptr)
		{
			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(m_vObjMat, matrixTranslation, matrixRotation, matrixScale);
			ImGui::InputFloat3("Tr", matrixTranslation);
			ImGui::InputFloat3("Rt", matrixRotation);
			ImGui::InputFloat3("Sc", matrixScale);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, m_vObjMat);
		}
	}
	if (ImGui::CollapsingHeader("File System"))
	{
		if (ImGui::Button("Save"))
		{
			//CFileSystem::Get_Instance()->Save_Map(TEXT("../Bin/Resources/SavedFiles/m.map"));
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			//CFileSystem::Get_Instance()->Load_Map(TEXT("../Bin/Resources/SavedFiles/m.map"));
		}
	}

}

