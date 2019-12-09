#include "Timer.h"

CTimer::CTimer()
	: m_SecondPerCount(0.0)
	, m_MainTime(0.0)
	, m_DeltaTime(0.0)
	, m_iBaseTime(0)
	, m_iPauseTime(0)
	, m_iStopTime(0)
	, m_iPrevTime(0)
	, m_iCurTime(0)
	, m_IsStopped(FALSE)
{
	__int64 countPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSec);
	m_SecondPerCount = 1.0 / static_cast<double>(countPerSec);
}

float CTimer::Get_TotalTime()
{
	if (TRUE == m_IsStopped)
	{
		return (float)(((m_IsStopped - m_iPauseTime) - m_iBaseTime) * m_SecondPerCount);
	}

	return (float)(((m_iCurTime - m_iPauseTime) - m_iBaseTime) * m_SecondPerCount);
}

void CTimer::Reset()
{
	__int64 curTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

	m_iBaseTime = curTime;
	m_iPrevTime = curTime;
	m_iStopTime = 0;
	m_IsStopped = FALSE;
}

void CTimer::Start()
{
	if (TRUE == m_IsStopped)
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		m_iPauseTime += startTime - m_iStopTime;
		
		m_iPrevTime = startTime;
		m_iStopTime = 0;
		m_IsStopped = FALSE;
	}
}

void CTimer::Stop()
{
	if (FALSE == m_IsStopped)
	{
		__int64 curTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&curTime);

		m_iStopTime = curTime;
		m_IsStopped = TRUE;
	}
}

void CTimer::Tick()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_iCurTime);

	m_MainTime = m_DeltaTime = (m_iCurTime - m_iPrevTime) * m_SecondPerCount;

	m_iPrevTime = m_iCurTime;

	if (TRUE == m_IsStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	if (0.0 > m_DeltaTime || 0.0 > m_MainTime)
	{
		m_MainTime = 0.0;
		m_DeltaTime = 0.0;
	}
}
