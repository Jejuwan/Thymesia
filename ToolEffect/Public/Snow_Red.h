#pragma once

#include "EffectTool_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(ToolEffect)

class CSnow_Red final : public CGameObject
{
private:
	CSnow_Red(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnow_Red(const CSnow_Red& rhs);
	virtual ~CSnow_Red() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:	
	CShader*						m_pShaderCom = { nullptr };	
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };
	

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSnow_Red* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;

private:
	_float4 m_Color = { 0,0,0,1 };
	_float2 m_Size = { 1,1 };
	_float m_fAlpha = 0.f;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC m_InitializeData;
public:
	void Set_Color(_float4 color) {
		m_Color = color;
		m_fAlpha = color.w;
	}
	CVIBuffer_Point_Instance::POINTINSTANCEDESC* Get_PointInstanceDesc() { return static_cast<CVIBuffer_Point_Instance*>(m_pVIBufferCom)->Get_InstanceData(); }
	void Set_Size(_float2 _size) { m_Size = _size; }
	void Set_InitializeData(CVIBuffer_Point_Instance::POINTINSTANCEDESC desc) { m_InitializeData = desc; }
};

END