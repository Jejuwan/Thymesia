#pragma once
#include "Base.h"
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)
class CGameObject;
class CTransform;
END

BEGIN(ToolEffect)
class CMapToolUI :
    public CBase
{
    DECLARE_SINGLETON(CMapToolUI)
private:
    CMapToolUI();
    ~CMapToolUI() = default;
    
public:
    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void Tick(_float fTimeDelta);
    void Render();

    virtual void Free() override;

    void UpdateUI();

    void SetSelectedObj(class CGameObject* pGameObject) { m_selectedObj = pGameObject; }



private:
    _int m_iNumVerticesX = { 0 };
    _int m_iNumVerticesZ = { 0 };
    class CGameObject* m_selectedObj = { nullptr };
    class CTransform* m_selectedObjTransform = { nullptr };
    _bool m_bClicked = { false };

    float m_vViewMat[16];
    float m_vProjMat[16];
	float m_vObjMat[16] =
	{ 1.f, 0.f, 0.f, 0.f,
0.f, 1.f, 0.f, 0.f,
0.f, 0.f, 1.f, 0.f,
0.f, 0.f, 0.f, 1.f };

    ImGuizmo::MODE m_mode = ImGuizmo::LOCAL;
    ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    vector<wstring> m_effectPath;
    int m_iCurrentEffectIdx = 0;

    class CSnow_Red* m_pEffect = { nullptr };
    CVIBuffer_Point_Instance::POINTINSTANCEDESC m_Desc;

    void Load_EffectList(wstring strPath);
};
END

