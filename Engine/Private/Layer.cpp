#include "Engine_Pch.h"
#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const wstring & strComTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)	
		++iter;

	return (*iter)->Find_Component(strComTag);	
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject->SetLayerIdx(m_iElemIdx++);
	m_GameObjects.push_back(pGameObject);
	return S_OK;
}

HRESULT CLayer::Delete_GameObject(_uint iIdx)
{
	auto it = m_GameObjects.begin();
	for (auto& obj : m_GameObjects)
	{
		if (obj->GetLayerIdx() == iIdx)
		{
			break;
		}
		else
			it++;
	}
	Safe_Release(*it);
	m_GameObjects.erase(it);
	return S_OK;
}

CGameObject* CLayer::Get_GameObject(_uint iIdx)
{
	if (m_GameObjects.empty())
		return nullptr;

	auto it = m_GameObjects.begin();
	while (iIdx--)
		it++;
	return *it;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		if ((*it)->GetDead())
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
		}
		else
		{
			(*it)->Tick(fTimeDelta);
			++it;
		}
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

CLayer * CLayer::Create()
{
	return new CLayer;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}


