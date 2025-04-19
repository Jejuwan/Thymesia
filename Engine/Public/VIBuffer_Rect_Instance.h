#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagRectInstanceDesc
	{
		char		szName[16];
		_float4		vColor;
		_float4		vPivotPos;
		_float3		vRange;
		_float		fLifeTime, fLifeMaxTime;
		_float3		fMinDir, fMaxDir;
		_float2		vSize;
		_float2		fSpeed;
		_float		fGravityForce;
		_float		fRelativeFallSpeed;
		_float		fDisappearTime;
	}RECTINSTANCEDESC;
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Tick(_float fTimeDelta);

private:


	RECTINSTANCEDESC			m_InstanceData;
	_float3*						m_pInstanceSpeed = { nullptr };

	mt19937_64					m_RandomNumber;
	uniform_real_distribution<float>		m_RandomX;
	uniform_real_distribution<float>		m_RandomY;
	uniform_real_distribution<float>		m_RandomZ;

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	RECTINSTANCEDESC* Get_InstanceData() { return &m_InstanceData; }
};

END