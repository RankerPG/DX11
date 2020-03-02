#include "Particle.h"

CParticle::CParticle(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pInitVB(nullptr)
	, m_pDrawVB(nullptr)
	, m_pStreamOutVB(nullptr)
	, m_dwMaxParticles(0)
	, m_isFirstRun(TRUE)
	, m_fGameTime(0.f)
	, m_fTimeStep(0.f)
	, m_fAge(0.f)
	, m_vecEyePosW(g_XMZero)
	, m_vecEmitPosW(g_XMZero)
	, m_vecEmitDirW(g_XMZero)
{

}

CParticle::CParticle(const CParticle& rhs)
	: CComponent(rhs)
	, m_pInitVB(rhs.m_pInitVB)
	, m_pDrawVB(rhs.m_pDrawVB)
	, m_pStreamOutVB(rhs.m_pStreamOutVB)
	, m_dwMaxParticles(rhs.m_dwMaxParticles)
	, m_isFirstRun(rhs.m_isFirstRun)
	, m_fGameTime(rhs.m_fGameTime)
	, m_fTimeStep(rhs.m_fTimeStep)
	, m_fAge(rhs.m_fAge)
	, m_vecEyePosW(rhs.m_vecEyePosW)
	, m_vecEmitPosW(rhs.m_vecEmitPosW)
	, m_vecEmitDirW(rhs.m_vecEmitDirW)
{
	m_pInitVB->AddRef();
	m_pDrawVB->AddRef();
	m_pStreamOutVB->AddRef();
}

CParticle::~CParticle()
{
	SAFE_RELEASE(m_pInitVB);
	SAFE_RELEASE(m_pDrawVB);
	SAFE_RELEASE(m_pStreamOutVB);
}

void CParticle::Create_Buffer()
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(PARTICLEVERTEX) * m_dwMaxParticles;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pStreamOutVB)))
	{
		MessageBox(g_hWnd, L"StreamOut Vertex Create Failed!!", 0, 0);
		return;
	}
}

void CParticle::Render_Particle()
{
	assert(m_pInitVB);
	assert(m_pDrawVB);
	assert(m_pStreamOutVB);

	m_pContext->SOSetTargets(1, &m_pStreamOutVB, 0);

	ID3D11Buffer* swapbuffer = nullptr;
	m_pContext->SOSetTargets(1, &swapbuffer, 0);

	m_pContext->DrawAuto();
}
