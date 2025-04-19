#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CTransform;
END

BEGIN(Client)
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

    void SelectObj();
    void DragSelectObj(_float fTimeDelta);
    void UpdateUI();

    void SetSelectedObj(class CCube* pGameObject) { m_selectedObj = pGameObject; }



private:
    void Load_ModelList(wstring strPath);
    void Load_MonsterList(wstring strPath);
private:
    _int m_iNumVerticesX = { 0 };
    _int m_iNumVerticesZ = { 0 };
    class CCube* m_selectedObj = { nullptr };
    class CTransform* m_selectedObjTransform = { nullptr };
    _bool m_bClicked = { false };

	float m_vObjMat[16] =
	{ 1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f };

    vector<wstring> m_propPath;
    vector<wstring> m_monsterPath;
    vector<wstring> m_levels = { TEXT("Lv1"),TEXT("Bat") };
    vector<wstring> m_monsterList;

    int m_iCurrentPropIdx = 0;
    int m_iCurrentMonsterIdx = 0;
    int m_iCurrentLevelIdx = 0;
    int m_iSpawnMonsterIdx = 0;

    ImGuizmo::MODE m_mode = ImGuizmo::LOCAL;
    ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
};
END

