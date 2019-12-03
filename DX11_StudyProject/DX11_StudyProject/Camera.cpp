#include "Camera.h"

CCamera::CCamera(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_vPos(XMVectorZero())
	, m_vTarget(XMVectorZero())
	, m_vUp(XMVectorZero())
	, m_matView(XMMatrixIdentity())
{
}

CCamera::~CCamera()
{
}

void CCamera::Init()
{
	m_vPos = XMVectorSet(0.f, 10.f, -20.f, 1.f);
	m_vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_matView = XMMatrixLookAtLH(m_vPos, m_vTarget, m_vUp);

	g_matView = m_matView;
}

void CCamera::Update(float p_deltaTime)
{
	XMVECTOR vRight, vUp, vLook;
	vRight = m_matView.r[0];
	vUp = m_matView.r[1];
	vLook = m_matView.r[2];

	float fSpeed = p_deltaTime * 10.f;

	if (GetAsyncKeyState('W'))
	{
		m_vPos += vLook * fSpeed;
		m_vTarget += vLook * fSpeed;
	}
	if (GetAsyncKeyState('S'))
	{
		m_vPos -= vLook * fSpeed;
		m_vTarget -= vLook * fSpeed;
	}
	if (GetAsyncKeyState('A'))
	{
		m_vPos -= vRight * fSpeed;
		m_vTarget -= vRight * fSpeed;
	}
	if (GetAsyncKeyState('D'))
	{
		m_vPos += vRight * fSpeed;
		m_vTarget += vRight * fSpeed;
	}

	m_matView = XMMatrixLookAtLH(m_vPos, m_vTarget, m_vUp);

	g_matView = m_matView;
}

void CCamera::Render()
{
}

void CCamera::Release()
{
}
