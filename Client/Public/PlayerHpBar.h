#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

class CPlayerHpBar :
    public CUI
{
private:
	CPlayerHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerHpBar(const CPlayerHpBar& rhs);
	virtual ~CPlayerHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

