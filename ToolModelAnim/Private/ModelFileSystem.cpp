#include "ToolModelAnim_Pch.h"
#include "MapTool_Defines.h"
#include "ModelFileSystem.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CModelFileSystem)


CModelFileSystem::CModelFileSystem()
{
}

CModelFileSystem::~CModelFileSystem()
{
}

void CModelFileSystem::Initialize()
{
	 //fstream os("../../SaveFiles/a.b",ios::out);
}

void CModelFileSystem::Save_Model(const wstring& strFilePath)
{
	//if (fout.is_open())
	//{
	//	CGameObject* pObj = CGameInstance::Get_Instance()->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model"),0);
	//	CModel* pModel = static_cast<CModel*>(pObj->Find_Component(TEXT("Com_Model")));
	//	/*_uint size = pLayer->size();
	//	fout.write((char*)&size, sizeof(_uint));
	//	for (_int i = 0; i < pLayer->size(); i++)
	//	{
	//		CGameObject* pGameObj = CGameInstance::Get_Instance()->Get_GameObject(LEVEL_GAMEPLAY, TEXT("LA"), i);
	//		CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
	//		fout.write((char*)&transform->Get_WorldMatrix(), sizeof(_float4x4));
	//	}*/
	//}
}

void CModelFileSystem::Load_Model(const wstring& strFilePath)
{
}

void CModelFileSystem::Free()
{
}
