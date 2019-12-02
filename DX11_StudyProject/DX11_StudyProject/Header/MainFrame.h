#pragma once

#include "framework.h"

class CDevice;
class CTimer;
class CBox;

class CMainFrame
{
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
	CDevice*				m_pDevice;
	shared_ptr<CTimer>		m_pMainTimer;
	shared_ptr<CBox>		m_pBox;

private:
	UINT					m_dwFrameCnt;
	float					m_fElapsedTime;
};

