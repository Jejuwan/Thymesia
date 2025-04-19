#include "Client_Pch.h"
#include "Client_Defines.h"
#include "FileSystem.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Cube.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CFileSystem)

CFileSystem::CFileSystem()
{
}

CFileSystem::~CFileSystem()
{

}

void CFileSystem::Save_Map(const wstring& strFilePath)
{
	ofstream fout(strFilePath,ios::binary | ios::out);

	CLayer* pLayer = CGameInstance::Get_Instance()->Get_Layer(LEVEL_MAPTOOL, TEXT("LA"));

	if(pLayer!=nullptr)
	{
		_uint size = pLayer->size();
		fout.write((char*)&size, sizeof(_uint));

		for (_uint i = 0; i < pLayer->size(); i++)
		{
			CCube* pGameObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_MAPTOOL, TEXT("LA"), i));

			size_t size = pGameObj->GetName()->size();
			fout.write((char*)&size, sizeof(size_t));

			char multiByteStr[100];
			WideCharToMultiByte(CP_UTF8, 0, pGameObj->GetName()->c_str(), -1, multiByteStr, sizeof(multiByteStr), nullptr, nullptr);
			fout.write((char*)multiByteStr, sizeof(char) * size);

			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
			fout.write((char*)&transform->Get_WorldMatrix(), sizeof(_float4x4));


			fout.write((char*)pGameObj->GetCooked(), sizeof(_bool));
		}
	}
	else
	{
		_int size = 0;
		fout.write((char*)&size, sizeof(_int));
	}


	CLayer* pMonLayer = CGameInstance::Get_Instance()->Get_Layer(LEVEL_MAPTOOL, TEXT("LM"));

	if(nullptr!=pMonLayer)
	{
		_uint pMonsize = pMonLayer->size();
		fout.write((char*)&pMonsize, sizeof(_uint));

		for (_uint i = 0; i < pMonLayer->size(); i++)
		{
			CCube* pGameObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_MAPTOOL, TEXT("LM"), i));

			size_t size = pGameObj->GetName()->size();
			fout.write((char*)&size, sizeof(size_t));

			char multiByteStr[100];
			WideCharToMultiByte(CP_UTF8, 0, pGameObj->GetName()->c_str(), -1, multiByteStr, sizeof(multiByteStr), nullptr, nullptr);
			fout.write((char*)multiByteStr, sizeof(char) * size);

			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
			fout.write((char*)&transform->Get_WorldMatrix(), sizeof(_float4x4));
		}
	}
	else
	{
		_int size = 0;
		fout.write((char*)&size, sizeof(_int));
	}

	fout.close();
}

void CFileSystem::Load_Map(const wstring& strFilePath)
{
	ifstream fin(strFilePath,ios::binary);
	_uint iSize;
	fin.read((char*)&iSize, sizeof(_uint));

	if(0!=iSize)
	{
		for (_uint i = 0; i < iSize; i++)
		{
			size_t size;
			fin.read((char*)&size, sizeof(size_t));

			char multiByteStr[100] = "";
			fin.read((char*)multiByteStr, sizeof(char) * size);

			wchar_t wideCharStr[100] = L"";
			MultiByteToWideChar(CP_ACP, 0, multiByteStr, (int)size, wideCharStr, 100);

			MODELDESC md;
			md.strPath = wideCharStr;

			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_MAPTOOL, TEXT("LA"), TEXT("Prototype_GameObject_Cube"), &md);
			CCube* pGameObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_MAPTOOL, TEXT("LA"), i));
			*pGameObj->GetName() = wideCharStr;
			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
			_float4x4 worldMat;
			fin.read((char*)&worldMat, sizeof(_float4x4));
			transform->Set_WorldMatrix(worldMat);
			_float3 vPos;
			XMStoreFloat3(&vPos, transform->Get_State(CTransform::STATE_POSITION));
			/*	pGameObj->Get_RigidBody()->setGlobalPose({ vPos.x,vPos.y,vPos.z });*/

			pGameObj->SetBoundingBox();
			fin.read((char*)pGameObj->GetCooked(), sizeof(_bool));

		}
	}

	CLayer* pMonLayer = CGameInstance::Get_Instance()->Get_Layer(LEVEL_MAPTOOL, TEXT("LM"));

	_uint iMonSize;
	fin.read((char*)&iMonSize, sizeof(_uint));

	if(0!=iMonSize)
	{
		for (_uint i = 0; i < iMonSize; i++)
		{
			size_t size;
			fin.read((char*)&size, sizeof(size_t));

			char multiByteStr[100] = "";
			fin.read((char*)multiByteStr, sizeof(char) * size);

			wchar_t wideCharStr[100] = L"";
			MultiByteToWideChar(CP_ACP, 0, multiByteStr, (int)size, wideCharStr, 100);

			MODELDESC md;
			md.strPath = wideCharStr;

			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_MAPTOOL, TEXT("LM"), TEXT("Prototype_GameObject_Cube"), &md);
			CCube* pGameObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_MAPTOOL, TEXT("LM"), i));
			*pGameObj->GetName() = wideCharStr;
			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
			_float4x4 worldMat;
			fin.read((char*)&worldMat, sizeof(_float4x4));
			transform->Set_WorldMatrix(worldMat);
			_float3 vPos;
			XMStoreFloat3(&vPos, transform->Get_State(CTransform::STATE_POSITION));
			/*	pGameObj->Get_RigidBody()->setGlobalPose({ vPos.x,vPos.y,vPos.z });*/

			pGameObj->SetBoundingBox();
		}
	}


	fin.close();
}

void CFileSystem::Load_Game(const wstring& strFilePath)
{
	ifstream fin(strFilePath, ios::binary);

	_uint iSize;
	fin.read((char*)&iSize, sizeof(_uint));
	streampos p = fin.tellg();

	if(iSize)
	{
		for (_uint i = 0; i < iSize; i++)
		{
			size_t size;
			fin.read((char*)&size, sizeof(size_t));

			char multiByteStr[100] = "";
			fin.read((char*)multiByteStr, sizeof(char) * size);

			wchar_t wideCharStr[100] = L"";
			MultiByteToWideChar(CP_ACP, 0, multiByteStr, (int)size, wideCharStr, 100);

			MODELDESC md;
			md.strPath = wideCharStr;

			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_GAMEPLAY, TEXT("LA"), TEXT("Prototype_GameObject_Cube"), &md);
			CCube* pGameObj = static_cast<CCube*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_GAMEPLAY, TEXT("LA"), i));
			*pGameObj->GetName() = wideCharStr;
			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));
			_float4x4 worldMat;
			fin.read((char*)&worldMat, sizeof(_float4x4));
			transform->Set_WorldMatrix(worldMat);

			pGameObj->SetBoundingBox();
			fin.read((char*)pGameObj->GetCooked(), sizeof(_bool));

			if (*pGameObj->GetCooked())
				pGameObj->Cook();

		}
	}

	_uint iMonSize;
	fin.read((char*)&iMonSize, sizeof(_uint));
	p = fin.tellg();

	if(iMonSize)
	{
		for (_uint i = 0; i < iMonSize; i++)
		{
			size_t size;
			fin.read((char*)&size, sizeof(size_t));

			char multiByteStr[100] = "";
			fin.read((char*)multiByteStr, sizeof(char) * size);

			char finalTag[100] = "Prototype_GameObject_";
			strcat_s(finalTag, sizeof(finalTag), multiByteStr);

			wchar_t wideCharStr[100] = L"";
			MultiByteToWideChar(CP_ACP, 0, finalTag, sizeof(finalTag), wideCharStr, 100);

			MODELDESC md;
			fin.read((char*)&md.initWorldMat, sizeof(_float4x4));

			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), wideCharStr, &md);
			CMonster* pGameObj = static_cast<CMonster*>(CGameInstance::Get_Instance()->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), i));
			CTransform* transform = static_cast<CTransform*>(pGameObj->Find_Component(TEXT("Com_Transform")));

		}
	}

	fin.close();
}

void CFileSystem::Free()
{
}
