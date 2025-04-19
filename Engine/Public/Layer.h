#pragma once

#include "Base.h"

/* 내가 나누고 싶은 기준으로 객체들을 모아두기위해. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const wstring& strComTag, _uint iIndex);

public:
	_uint size() { return (_uint)m_GameObjects.size(); }
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Delete_GameObject(_uint iIdx);
	class CGameObject* Get_GameObject(_uint iIdx);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private:
	list<class CGameObject*>			m_GameObjects;
	int m_iElemIdx = 0;
public:
	static CLayer* Create();
	virtual void Free() override;
};

END