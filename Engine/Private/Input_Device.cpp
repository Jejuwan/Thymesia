#include "Engine_Pch.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

void CInput_Device::Tick()
{
	for (_uint i = 0; i < 4; i++)
	{
		if (Get_DIMouseState((MOUSEKEYSTATE)i))
		{
			if (m_MouseKey[i].first)
			{
				m_MouseKey[i].second = KEY_PRESSING;
			}
			else
			{
				m_MouseKey[i].first = true;
				m_MouseKey[i].second = KEY_DOWN;
			}
		}
		else
		{
			if (m_MouseKey[i].first)
			{
				m_MouseKey[i].first = false;
				m_MouseKey[i].second = KEY_UP;
			}
			else
			{
				m_MouseKey[i].second = KEY_NONE;
			}
		}
	}

	for (_int i = 0; i < 255; i++)
	{
		if (GetAsyncKeyState(i)&0x8000)
		{
			if (m_KeyboardKey[i].first)
			{
				m_KeyboardKey[i].second = KEY_PRESSING;
			}
			else
			{
				m_KeyboardKey[i].first = true;
				m_KeyboardKey[i].second = KEY_DOWN;
			}
		}
		else
		{
			if (m_KeyboardKey[i].first)
			{
				m_KeyboardKey[i].first = false;
				m_KeyboardKey[i].second = KEY_UP;
			}
			else
			{
				m_KeyboardKey[i].second = KEY_NONE;
			}
		}
	}

}

HRESULT CInput_Device::Ready_DInput(HINSTANCE hInst, HWND hWnd)
{
	// �θ� �İ�ü ����
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;


	// ������ Ű���� ��ü�� ���� ������ �İ�ü���� ����
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ������ Ű���� ��ü�� ���� ����(��������) �� ����
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ Ű���� �İ�ü�� Ȱ��ȭ
	m_pKeyBoard->Acquire();

	// �� ��ǻ�Ϳ� ����Ǿ� �ִ� ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �İ�ü���� ����
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ������ ���콺 ��ü�� ���� ������ ����
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ ���콺 �İ�ü�� Ȱ��ȭ
	m_pMouse->Acquire();


	for (auto& iter : m_MouseKey)
		iter = { false, KEY_NONE };

	return S_OK;
}

void CInput_Device::Update_DInput(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
