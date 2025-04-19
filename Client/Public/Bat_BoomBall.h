#pragma once

#include "Client_Defines.h"
#include "Client_Struct.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CBat_BoomBall :
    public CMonster
{
protected:
	CBat_BoomBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBat_BoomBall(const CBat_BoomBall& rhs);
	virtual ~CBat_BoomBall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CShader* m_pAlphaShaderCom = { nullptr };

	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBat_BoomBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};
END