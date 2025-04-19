#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState);
	_float3 Get_Scaled();
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}
	void Set_WorldMatrix(_float4x4 m)
	{
		m_WorldMatrix = m;
	}
	_float4x4 Get_WorldMatrix()
	{
		return m_WorldMatrix;
	}
public:
	void Set_State(STATE eState, _fvector vState);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const char* pConstantName);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Dir(_float3 vDir, _float fSpeed, _float fTimeDelta);
	void Go_Pos(_float3 vDir, _float fSpeed, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Scaled(const _float3& vScale);
	void Chase(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimit = 0.f);
	void LookAt(_fvector vTargetPos);
	void LookAtDir(_fvector lookDir);
	void CopyRotation(CTransform* pTrans);
	_float Distance(CTransform* trans);
	_vector Get_HoriziontalLookAtDir(CTransform* trans);
	_vector Get_HoriziontalLookDir();

private:
	TRANSFORMDESC				m_TransformDesc;
	// _vector XMVECTOR : SIMD������ ���� ��ȵ� Ÿ��. 
	// float4�� ��ŭ�� ������ ������. ���� ����� �׹��� ����(x) �ѹ��� ����(o)
	// Stack�޸𸮿� ����Ǿ���Ѵ�. 
	// �ּҸ� ���� ����(x)
	// _matrix XMMATRIX : SIMD������ ���� ��ȵ� Ÿ��.
	// XMVECTOR�� 4�� ��ŭ(float16��)�� ������ ������.Stack�޸𸮿� ����Ǿ���Ѵ�. 
	// �ּҸ� ���� ����(x)
	// _float4x4 XMFLOAT4X4 : ������ ���� ���. �ּҸ� ���� ����(o)
	_float4x4					m_WorldMatrix;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END