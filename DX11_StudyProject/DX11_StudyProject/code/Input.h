#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")

class CInput
{
public:
	static CInput* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CInput;
		}

		return m_pInstance;
	}

	static void Release_Instance()
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CInput* m_pInstance;

public:
	enum MOUSESTATE { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON, DIM_END };
	enum MOUSEMOVESTATE { DIMMS_X, DIMMS_Y, DIMMS_WHEEL };

private:
	CInput();
	~CInput();

public:
	inline char Get_DIKState(const unsigned char& byKey) {
		return m_byKeyBoard_State[byKey];
	}

	inline BOOL Get_DIKPressState(const unsigned char& byKey) {
		return m_bPressedKey[byKey];
	}

	inline BOOL Get_DIMState(const MOUSESTATE & eMouseState) {
		return m_MouseState.rgbButtons[eMouseState];
	}

	inline LONG Get_DIMMoveState(const MOUSEMOVESTATE & eMouseMoveState) {
		return *((LONG*)&m_MouseState + eMouseMoveState);
	}

	inline BOOL Get_DIMPressState(const MOUSESTATE & eMouseState) {
		return m_bPressingMouseKey[eMouseState];
	}

public:
	HRESULT Init_Input_Device(HINSTANCE hInst, HWND hWnd);
	void Update_InputDev_State();

private:
	HRESULT Init_KeyBoard(HWND hWnd);
	HRESULT Init_Mouse(HWND hWnd);
private:
	LPDIRECTINPUT8				m_pInputSDK;
private:
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	char						m_byKeyBoard_State[256];
	BOOL						m_bPressedKey[256];
private:
	LPDIRECTINPUTDEVICE8		m_pMouse;
	DIMOUSESTATE				m_MouseState;
	BOOL						m_bPressingMouseKey[3];
};

