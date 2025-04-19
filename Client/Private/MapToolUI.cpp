#include "Client_Pch.h"
#include "Client_Defines.h"
#include "..\Public\MapToolUI.h"
#include "GameInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Layer.h"
#include "Cube.h"
#include "Camera_Free.h"
#include "FileSystem.h"

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

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	ImGuizmo::SetOrthographic(false);

	m_propPath.clear();
	Load_ModelList(TEXT("../../SaveFiles/Models/Props"));
	Load_MonsterList(TEXT("../../SaveFiles/Models/Monsters"));
}

void CMapToolUI::Tick(_float fTimeDelta)
{
	SelectObj();
	DragSelectObj(fTimeDelta);
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

void CMapToolUI::SelectObj()
{
	if (CGameInstance::Get_Instance()->Mouse_Down(CInput_Device::DIMK_LBUTTON) && !ImGui::GetIO().WantCaptureMouse)
	{
		m_selectedObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Intersect_Ray_Layer(LEVEL_MAPTOOL, TEXT("LA")));
		if (m_selectedObj != nullptr)
		{
			m_selectedObjTransform = static_cast<CTransform*>(m_selectedObj->Find_Component(TEXT("Com_Transform")));
			m_bClicked = true;

			_float4x4 mat = m_selectedObjTransform->Get_WorldMatrix();
			for (int i = 0; i < 4; i++)
				memcpy(&m_vObjMat[i * 4], &mat.m[i], sizeof(float) * 4);
		}
		else
		{
			m_selectedObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Intersect_Ray_Layer(LEVEL_MAPTOOL, TEXT("LM")));
			if (m_selectedObj != nullptr)
			{
				m_selectedObjTransform = static_cast<CTransform*>(m_selectedObj->Find_Component(TEXT("Com_Transform")));
				m_bClicked = true;

				_float4x4 mat = m_selectedObjTransform->Get_WorldMatrix();
				for (int i = 0; i < 4; i++)
					memcpy(&m_vObjMat[i * 4], &mat.m[i], sizeof(float) * 4);
			}
		}
	}
}

void CMapToolUI::DragSelectObj(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Mouse_Up(CInput_Device::DIMK_LBUTTON))
	{
		m_bClicked = false;
	}

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
		static_cast<CCube*>(m_selectedObj)->SetBoundingBox();
	}

 	ImGui::Begin("Scene Control");


	if (ImGui::CollapsingHeader("Terrain"))
	{
		ImGui::InputInt(
			"TerrainX",
			&m_iNumVerticesX);
		ImGui::InputInt(
			"TerrainY",
			&m_iNumVerticesZ);
	}
	if (ImGui::CollapsingHeader("GameObject"))
	{
		if (ImGui::BeginListBox("Props"))
		{
		
			for (int n = 0; n < m_propPath.size(); n++)
			{
				string item(m_propPath[n].begin(), m_propPath[n].end());

				const bool is_selected = (m_iCurrentPropIdx == n);
				if (ImGui::Selectable(item.c_str(), is_selected))
					m_iCurrentPropIdx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Create"))
		{
			MODELDESC md;
			md.strPath = m_propPath[m_iCurrentPropIdx];
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_MAPTOOL, TEXT("LA"),
				TEXT("Prototype_GameObject_Cube"), &md); 
			m_selectedObj = static_cast<CCube*>(pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("LA"), pGameInstance->Get_Layer(LEVEL_MAPTOOL, TEXT("LA"))->size() - 1));
			*m_selectedObj->GetName() = md.strPath;
			CTransform* transform = static_cast<CTransform*>(m_selectedObj->Get_Component(TEXT("Com_Transform")));
			transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pGameInstance->Get_Create_Pos(10.f)));
			m_selectedObjTransform = transform;
			_float4x4 mat = transform->Get_WorldMatrix();
			for (int i = 0; i < 4; i++)
				memcpy(&m_vObjMat[i*4], &mat.m[i], sizeof(float) * 4); 
			m_vObjMat[15] = 1;	
			static_cast<CCube*>(m_selectedObj)->SetBoundingBox();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			if (m_selectedObj != nullptr)
			{
				CGameInstance::Get_Instance()->Delete_GameObject(LEVEL_MAPTOOL, TEXT("LA"),
					m_selectedObj->GetLayerIdx());
				m_selectedObj = nullptr;
				m_selectedObjTransform = nullptr;
			}
		}
		if (m_selectedObj != nullptr)
			ImGui::Checkbox("Cooked", m_selectedObj->GetCooked());
	}
	if (ImGui::CollapsingHeader("Monster"))
	{
		if (ImGui::BeginListBox("Monsters"))
		{
			for (int n = 0; n < m_monsterPath.size(); n++)
			{
				string item(m_monsterPath[n].begin(), m_monsterPath[n].end());

				const bool is_selected = (m_iCurrentMonsterIdx == n);
				if (ImGui::Selectable(item.c_str(), is_selected))
					m_iCurrentMonsterIdx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("Create2"))
		{
			MODELDESC md;
			md.strPath = m_monsterPath[m_iCurrentMonsterIdx];
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_MAPTOOL, TEXT("LM"),
				TEXT("Prototype_GameObject_Cube"), &md);
			m_selectedObj = static_cast<CCube*>(pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("LM"), pGameInstance->Get_Layer(LEVEL_MAPTOOL, TEXT("LM"))->size() - 1));
			*m_selectedObj->GetName() = md.strPath;
			CTransform* transform = static_cast<CTransform*>(m_selectedObj->Get_Component(TEXT("Com_Transform")));
			m_selectedObjTransform = transform;
			_float4x4 mat = transform->Get_WorldMatrix();
			for (int i = 0; i < 4; i++)
				memcpy(&m_vObjMat[i * 4], &mat.m[i], sizeof(float) * 4);
			m_vObjMat[15] = 1;
			static_cast<CCube*>(m_selectedObj)->SetBoundingBox();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete2"))
		{
			if (m_selectedObj != nullptr)
			{
				CGameInstance::Get_Instance()->Delete_GameObject(LEVEL_MAPTOOL, TEXT("LM"),
					m_selectedObj->GetLayerIdx());
				m_selectedObj = nullptr;
				m_selectedObjTransform = nullptr;
			}
		}

		if (ImGui::BeginListBox("SpawnedMonsters"))
		{
			CLayer* monLayer = pGameInstance->Get_Layer(LEVEL_MAPTOOL, TEXT("LM"));
			if(nullptr!=monLayer)
			{
				_int size = monLayer->size();
				for (int n = 0; n < size; n++)
				{
					CCube* pElem = static_cast<CCube*>(pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("LM"),n));
					string item(pElem->GetName()->begin(), pElem->GetName()->end());

					const bool is_selected = (m_iSpawnMonsterIdx == n);
					if (ImGui::Selectable(item.c_str(), is_selected))
					{
						m_iSpawnMonsterIdx = n;
						m_selectedObj = pElem;
						m_selectedObjTransform = static_cast<CTransform*>(m_selectedObj->Find_Component(TEXT("Com_Transform")));
					
						_float4x4 mat = m_selectedObjTransform->Get_WorldMatrix();
						for (int i = 0; i < 4; i++)
							memcpy(&m_vObjMat[i * 4], &mat.m[i], sizeof(float) * 4);
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}

	}
	if (ImGui::CollapsingHeader("File System"))
	{
		if (ImGui::BeginListBox("Levels"))
		{
			for (int n = 0; n < m_levels.size(); n++)
			{
				string item(m_levels[n].begin(), m_levels[n].end());

				const bool is_selected = (m_iCurrentLevelIdx == n);
				if (ImGui::Selectable(item.c_str(), is_selected))
				{
					m_iCurrentLevelIdx = n;
					wstring modelPath = TEXT("../../SaveFiles/Models/Props/");
					modelPath += m_levels[m_iCurrentLevelIdx];
					m_propPath.clear();
					Load_ModelList(modelPath);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		wstring path = TEXT("../../SaveFiles/Maps/");
		path += m_levels[m_iCurrentLevelIdx] + TEXT(".map");
		if (ImGui::Button("Save"))
		{
			CFileSystem::Get_Instance()->Save_Map(path);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			m_selectedObj = nullptr;
			pGameInstance->Clear(LEVEL_MAPTOOL);

			CCamera_Free::CAMERAFREEDESC		CameraFreeDesc;

			//CameraFreeDesc.iData = 10;
			CameraFreeDesc.CameraDesc.iLevelIndex = LEVEL_STATIC;
			CameraFreeDesc.CameraDesc.strTranformTag = TEXT("Prototype_Component_Transform");
			CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
			CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
			CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
			CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
			CameraFreeDesc.CameraDesc.fNear = 0.2f;
			CameraFreeDesc.CameraDesc.fFar = 300.f;
			CameraFreeDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
			CameraFreeDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

			/* For.Camera */
			pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"),
				TEXT("Prototype_GameObject_Camera_Free"), &CameraFreeDesc);
			CFileSystem::Get_Instance()->Load_Map(path);
		}
	}

	//if (ImGui::CollapsingHeader("Light"))
	//{
	//	const LIGHTDESC* lightDesc = pGameInstance->Get_LightDesc(0);
	//	if (nullptr != lightDesc)
	//	{
	//		ImGui::DragFloat("Light X", &lightDesc->vDirection.x, 0.005f);
	//		ImGui::DragFloat("Light Y", &lightDesc->vDirection.y, 0.005f);
	//		ImGui::DragFloat("Light Z", &lightDesc->vDirection.z, 0.005f);
	//	}
	//}

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

void CMapToolUI::Load_ModelList(wstring strPath)
{
	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_ModelList(entry.path().wstring());
		else if (is_regular_file(entry) && entry.path().extension() == ".model") {
			wstring fileName = entry.path().stem().wstring();
			m_propPath.push_back(fileName);
		}
	}
}

void CMapToolUI::Load_MonsterList(wstring strPath)
{
	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_MonsterList(entry.path().wstring());
		else if (is_regular_file(entry) && entry.path().extension() == ".model") {
			wstring fileName = entry.path().stem().wstring();
			m_monsterPath.push_back(fileName);
		}
	}
}

