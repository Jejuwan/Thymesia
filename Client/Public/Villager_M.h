#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)
class CVillager_M :
    public CMonster
{
private:
    CVillager_M(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVillager_M(const CVillager_M& rhs);
    virtual ~CVillager_M() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
    virtual HRESULT Render();

private:
    HRESULT Add_Components();

public:
    static CVillager_M* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg)	override;
    virtual void Free() override;
};
END
