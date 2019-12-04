#include "Input.h"
#include "framework.h"

CInput* CInput::m_pInstance = nullptr;

CInput::CInput()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(&m_bPressingKey, sizeof(BOOL) * 256);
}

CInput::~CInput()
{
	SAFE_RELEASE(m_pKeyBoard);
	SAFE_RELEASE(m_pMouse);
	SAFE_RELEASE(m_pInputSDK);
}

HRESULT CInput::Init_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8
		, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(Init_KeyBoard(hWnd)))
		return E_FAIL;

	if (FAILED(Init_Mouse(hWnd)))
		return E_FAIL;

	return S_OK;
}

void CInput::Update_InputDev_State()
{
	if (nullptr == m_pKeyBoard)
		return;

	if (nullptr == m_pMouse)
		return;

	char byTempKeyState[256];
	memcpy(&byTempKeyState, &m_byKeyBoard_State, sizeof(char) * 256);

	m_pKeyBoard->GetDeviceState(256, m_byKeyBoard_State);

	for (size_t i = 0; i < 256; ++i)
	{
		if (m_byKeyBoard_State[i] & 0x80)
		{
			if (byTempKeyState[i] && m_byKeyBoard_State[i])
				m_bPressingKey[i] = true;
		}
		else
			m_bPressingKey[i] = false;
	}

	char byTempMouseState[3];

	for (size_t i = 0; i < 3; ++i)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)
		{
			if (byTempMouseState[i] && m_MouseState.rgbButtons[i])
				m_bPressingMouseKey[i] = true;
		}
		else
			m_bPressingMouseKey[i] = false;
	}

	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

HRESULT CInput::Init_KeyBoard(HWND hWnd)
{
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->Acquire()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInput::Init_Mouse(HWND hWnd)
{
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}