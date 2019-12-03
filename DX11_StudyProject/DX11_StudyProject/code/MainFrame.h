#pragma once

#include "framework.h"

class CDevice;
class CTimer;
class CComponent;
class CBox;
class CTerrain;
class CCamera;

class CMainFrame
{
	typedef unordered_map<LPCSTR, shared_ptr<CComponent>> COMHASHMAP;

public:
	CMainFrame(CDevice* p_Device);
	~CMainFrame();

public:
	void Init();
	void Update();
	void Render();
	void Release();

private:
	void Calculate_FPS();

private:
	void Create_RasterizerState();
	void Update_State();

private:
	CDevice*				m_pGraphicDevice;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	ID3D11RasterizerState*	m_pState;
	
	COMHASHMAP				m_mapComponent;

	shared_ptr<CCamera>		m_pCamera;
	shared_ptr<CTimer>		m_pMainTimer;
	shared_ptr<CBox>		m_pBox;
	shared_ptr<CTerrain>	m_pTerrain;

private:
	UINT					m_dwFrameCnt;
	float					m_fElapsedTime;
};

