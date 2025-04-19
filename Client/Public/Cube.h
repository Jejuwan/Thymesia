#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CCube final : public CGameObject
{
private:
	CCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCube(const CCube& rhs);
	virtual ~CCube() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow();

private:
	CShader* m_pShaderCom = { nullptr };
	//CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	_bool m_bClicked = { false };
	_bool m_bCooked = { true };
	wstring m_modelName = L"";
	static _int  m_iCloneNum;
private:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;

public:
	_bool* GetCooked() { return &m_bCooked; }
	wstring* GetName() { return &m_modelName; }
public:
	void Cook();
	void SetBoundingBox();
};

END