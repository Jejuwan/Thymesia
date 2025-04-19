#pragma once

#include "Client_Defines.h"
#include "Client_Struct.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CMonster : public CGameObject,
	public PxUserControllerHitReport
{
public:
	enum MON_TYPE {
		VILLAGER_M, VILLAGER_F, JOKER, GARDENER , B_BAT
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	vector<_bool> m_pattern = { false,false,false,false,false };
	_bool m_bScream = { false };
	_bool m_bDie = { false };
protected:
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pNonAnimShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CModel* m_pWeaponRModelCom = { nullptr };
	CTransform* m_pWeaponRTransformCom = { nullptr };

	
	PxController* m_pController = { nullptr };

	vector<ATTACKDESC*> m_pAttackDesc;

	class CMonsterState*			m_CurState = { nullptr };

	MON_TYPE m_eMonsterType;

	_bool m_bDead = { false };
	_bool m_bMonsterHit = { false };
	_int m_iHp = { 100 };
	_int m_iWeaponR_BoneIdx = { 0 };
	_int m_iHpBarIdx = { -1 };

	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_WeaponR_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;

public:
	void Entry_Dead();

public:
	PxController* Get_Controller() { return m_pController; }
	vector<ATTACKDESC*> Get_AttackDesc() { return m_pAttackDesc; }

	CModel* Get_Model() { return m_pModelCom; }
	CMonsterState* Get_MonsterState() { return m_CurState; }
	_bool Get_MonsterHit() { return m_bMonsterHit; }
	void Set_MonsterHit(_bool b) { m_bMonsterHit = b; }
	_int Get_Hp() { return m_iHp; }
	void Set_HP(_int i) { m_iHp = i; }
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool b) { m_bDead = b; }
	MON_TYPE Get_MonsterType() { return m_eMonsterType; }
	void Set_MonsterType(MON_TYPE type) { m_eMonsterType = type; }
	_int Get_HpBarIdx() { return m_iHpBarIdx; }
	void Set_HpBarIdx(_int idx) { m_iHpBarIdx = idx; }
public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) {};
	virtual void onControllerHit(const PxControllersHit& hit);
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) {};
};

END