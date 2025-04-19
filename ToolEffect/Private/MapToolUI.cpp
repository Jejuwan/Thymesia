#include "ToolModelEffect_Pch.h"
#include "EffectTool_Defines.h"
#include "..\Public\MapToolUI.h"
#include "GameInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Layer.h"
#include "Cube.h"
#include "Snow_Red.h"
#include "Camera_Free.h"
#include "Camera.h"

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

	m_effectPath.clear();
	Load_EffectList(TEXT("../../SaveFiles/Particles"));
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


	
 	ImGui::Begin("Scene Control");

	static char effectName[16] = {};
	static _float pivotPos[3] = { 0,0,0 };
	static _float vRange[3] = { 0,0,0 };
	static _float fLifeTime = 1.f;
	static ImVec4 color = ImVec4(1, 0, 0, 1);
	static _float fMinSpeed[3] = { -5.f,-5.f,-5.f }, fMaxSpeed[3] = { 5.f,5.f,5.f };
	static _float vSize[2] = { 1,1 };
	static _float vSpeed[2] = { 0,10 };
	static _float fGravityForce = { 0 };
	static _float fRelativeFallSpeed = { 0 };
	static _float fDisappearTime = { 0 };

	if (ImGui::CollapsingHeader("Effect"))
	{

		ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar |
			ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB;
		ImGui::ColorPicker4("MyColor##4", (float*)&color, flags);
		ImGui::InputText("Name", effectName, IM_ARRAYSIZE(effectName));
		ImGui::SliderFloat3("Pivot Pos", pivotPos, 0.0f, 50.0f);
		ImGui::SliderFloat3("Range", vRange, 0.0f, 50.0f);
		ImGui::SliderFloat("LifeTime", &fLifeTime, 0.0f, 10.0f);
		ImGui::InputFloat3("MinSpeed", fMinSpeed);
		ImGui::InputFloat3("MaxSpeed", fMaxSpeed);
		ImGui::InputFloat2("Size",vSize);
		ImGui::InputFloat2("Speed", vSpeed);
		ImGui::InputFloat("GravityForce", &fGravityForce);
		ImGui::InputFloat("RelativeFallSpeed", &fRelativeFallSpeed);
		ImGui::InputFloat("DisappearTime", &fDisappearTime);

		strcpy_s(m_Desc.szName, effectName);
		m_Desc.vColor = { color.x,color.y,color.z,color.w };
		m_Desc.vPivotPos = { pivotPos[0],pivotPos[1],pivotPos[2],1 };
		m_Desc.vRange = { vRange[0],vRange[1],vRange[2] };
		m_Desc.fLifeMaxTime = fLifeTime;
		m_Desc.fMinDir = { fMinSpeed[0], fMinSpeed[1], fMinSpeed[2] };
		m_Desc.fMaxDir = { fMaxSpeed[0], fMaxSpeed[1], fMaxSpeed[2] };
		m_Desc.vSize = { vSize[0],vSize[1] };
		m_Desc.fSpeed = { vSpeed[0],vSpeed[1] };
		m_Desc.fGravityForce = { fGravityForce };
		m_Desc.fRelativeFallSpeed = { fRelativeFallSpeed };
		m_Desc.fDisappearTime = { fDisappearTime };

		if(ImGui::Button("Create"))
		{
			pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Snow_Red"), &m_Desc);
			
			m_pEffect = static_cast<CSnow_Red*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), 0));
			
			if (nullptr != m_pEffect)
			{
				m_pEffect->Set_Color({ color.x, color.y, color.z, color.w });
				m_pEffect->Set_Size({ vSize[0],vSize[1] });
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("CamSet"))
		{
			CCamera_Free* pCamera = static_cast<CCamera_Free*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0));
			pCamera->Get_Transform()->Set_State(CTransform::STATE_POSITION, { 0,0,0,1 });
			pCamera->Get_Transform()->LookAtDir({ 0,0,1,0 });
		}

	}
	if (ImGui::CollapsingHeader("File System"))
	{
		if (ImGui::BeginListBox("Monsters"))
		{
			for (int n = 0; n < m_effectPath.size(); n++)
			{
				string item(m_effectPath[n].begin(), m_effectPath[n].end());

				const bool is_selected = (m_iCurrentEffectIdx == n);
				if (ImGui::Selectable(item.c_str(), is_selected))
					m_iCurrentEffectIdx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Save"))
		{
			char str[64] = "../../SaveFiles/Particles/";
			strcat_s(str, effectName);
			strcat_s(str, ".effect");
			ofstream os(str, ios::binary);
			os.write((char*)&m_Desc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
			os.close();
			m_effectPath.clear();
			Load_EffectList(TEXT("../../SaveFiles/Particles"));
		}
		if (ImGui::Button("Load"))
		{
			CVIBuffer_Point_Instance::POINTINSTANCEDESC* pDesc = { nullptr };
			wstring wstr = TEXT("../../SaveFiles/Particles/");
			wstr += wstring(m_effectPath[m_iCurrentEffectIdx].c_str());
			wstr += TEXT(".effect");
			ifstream is(wstr, ios::binary);
			is.read((char*)&m_Desc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
			is.close();

			strcpy_s(effectName, m_Desc.szName);
			color = ImVec4(m_Desc.vColor.x, m_Desc.vColor.y, m_Desc.vColor.z, m_Desc.vColor.w);
			pivotPos[0] = m_Desc.vPivotPos.x, pivotPos[1] = m_Desc.vPivotPos.y, pivotPos[2] = m_Desc.vPivotPos.z;
			vRange[0] = m_Desc.vRange.x, vRange[1] = m_Desc.vRange.y, vRange[2] = m_Desc.vRange.z;
			fLifeTime = m_Desc.fLifeMaxTime;
			fMinSpeed[0] = m_Desc.fMinDir.x, fMinSpeed[1] = m_Desc.fMinDir.y, fMinSpeed[2] = m_Desc.fMinDir.z;
			fMaxSpeed[0] = m_Desc.fMaxDir.x, fMaxSpeed[1] = m_Desc.fMaxDir.y, fMaxSpeed[2] = m_Desc.fMaxDir.z;
			vSize[0] = m_Desc.vSize.x, vSize[1] = m_Desc.vSize.y;
			vSpeed[0] = m_Desc.fSpeed.x, vSpeed[1] = m_Desc.fSpeed.y;
			fGravityForce = m_Desc.fGravityForce;
			fRelativeFallSpeed = m_Desc.fRelativeFallSpeed;
			fDisappearTime = m_Desc.fDisappearTime;
		}

	}
}

void CMapToolUI::Load_EffectList(wstring strPath)
{
	for (const auto& entry : directory_iterator(strPath)) {
		if (is_directory(entry.status()))
			Load_EffectList(entry.path().wstring());
		else if (is_regular_file(entry) && entry.path().extension() == ".effect") {
			wstring fileName = entry.path().stem().wstring();
			m_effectPath.push_back(fileName);
		}
	}
}

