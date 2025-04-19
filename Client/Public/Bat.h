#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)
class CBat :
    public CMonster, PxControllerFilterCallback
{
private:
    CBat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBat(const CBat& rhs);
    virtual ~CBat() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
    
    _int m_iHead_BoneIdx = -1;
    _int m_iArmR_BoneIdx = -1;
    _int m_iArmL_BoneIdx = -1;

    _bool m_bHit = false;
    _bool m_bPhaseTurn = false;
private:
	HRESULT Add_Components();

public:
    static CBat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg)	override;
    virtual void Free() override;
    virtual bool filter(const PxController& a, const PxController& b);

    _bool Get_Hit() { return m_bHit; }
    void Set_Hit(_bool b) { m_bHit = b; }
    _bool GetPhaseTurn() { return m_bPhaseTurn; }
    void SetPhaseTurn(_bool b) { m_bPhaseTurn = b; }
};
END

