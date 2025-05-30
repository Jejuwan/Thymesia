#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Clear_Lights();
private:
	list<class CLight*>			m_Lights;

private:
	virtual void Free() override;
};

END