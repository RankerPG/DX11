#pragma once

#include "framework.h"

class CDevice
{
public:
	static CDevice* Get_Instance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CDevice;
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
	static CDevice* m_pInstance;

private:
	CDevice();
	~CDevice();

public:
	inline ID3D11Device* Get_Device() { return m_pDeivce; } //가지고 있을 땐 AddRef 필요
	inline ID3D11DeviceContext* Get_Context() { return m_pContext; } //가지고 있을 땐 AddRef 필요

public:
	BOOL Init_Device(UINT p_dwSizeX, UINT p_dwSizeY);

	void Begin_Render();
	void End_Render();

private:
	BOOL Create_Device();
	BOOL Create_SwapChain();
	BOOL Create_RenderTargetView();
	BOOL Create_DepthStencilView();

	BOOL Init_Viewport();

private:
	ID3D11Device*				m_pDeivce;
	ID3D11DeviceContext*		m_pContext;
	
	IDXGISwapChain*				m_pSwapChain;
	
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11DepthStencilView*		m_pDepthStencilView;

private:
	XMFLOAT4					m_ClearColor;

	UINT						m_dwSizeX;
	UINT						m_dwSizeY;

	UINT						m_dw4xMSAAQuality;
};

