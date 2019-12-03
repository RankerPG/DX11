#include "MainFrame.h"
#include "device.h"
#include "timer.h"
#include "box.h"

CMainFrame::CMainFrame(CDevice* p_Device)
	: m_pDevice(p_Device)
	, m_pMainTimer(new CTimer())
	, m_dwFrameCnt(0)
	, m_fElapsedTime(0.f)
{
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::Init()
{
	if (FALSE == m_pDevice->Init_Device(1280, 720))
	{
		m_pDevice->Release_Instance();
		return;
	}

	m_pMainTimer->Reset();

	m_pBox.reset(new CBox(m_pDevice->Get_Device(), m_pDevice->Get_Context()));

	m_pBox->Init();
}

void CMainFrame::Update()
{
	m_pMainTimer->Tick();
	Calculate_FPS();
	float deltaTime = m_pMainTimer->Get_DeltaTime();

	m_pBox->Update(deltaTime);
}

void CMainFrame::Render()
{
	m_pDevice->Begin_Render();

	// ±×¸®±â
	m_pBox->Render();

	m_pDevice->End_Render();
}

void CMainFrame::Release()
{
}

void CMainFrame::Calculate_FPS()
{
	++m_dwFrameCnt;
	m_fElapsedTime += m_pMainTimer->Get_DeltaTime();

	if (m_fElapsedTime >= 1.f)
	{
		float fFrameTime = 1000.f / m_dwFrameCnt;

		wchar_t wText[64] = L"";
		_stprintf_s(wText, L"FPS : %d / Frame Time : %f(ms)", m_dwFrameCnt, fFrameTime);
		SetWindowText(g_hWnd, wText);

		m_dwFrameCnt = 0;
		m_fElapsedTime = 0.f;
	}
}
