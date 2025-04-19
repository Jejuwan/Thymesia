#pragma once

#include "MapTool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(ToolModel)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };
	CRenderer*					m_pRenderer = { nullptr };

private:
	HRESULT Open_Level(LEVEL eLevel);
	/* ��緹������ ����ϱ����� ������Ʈ ������ �����Ѵ�. */
	HRESULT Ready_Prototype_Component_ForStatic();
	/*HRESULT Ready_Prototype_GameObject_ForStatic();*/

public:
	static CMainApp* Create();
	virtual void Free() override;	
};

END

