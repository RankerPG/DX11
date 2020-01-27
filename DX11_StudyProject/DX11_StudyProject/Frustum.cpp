#include "Frustum.h"

CFrustum::CFrustum(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_vPoints{ XMVECTOR(), }
	, m_vWorldPoints{ XMVECTOR(), }
{
	m_pFrustumPlane = new XMVECTOR[6];
}

CFrustum::CFrustum(const CFrustum& rhs)
	: CComponent(rhs)
	, m_vPoints{ XMVECTOR(), }
	, m_vWorldPoints{ XMVECTOR(), }
	, m_pFrustumPlane(rhs.m_pFrustumPlane)
{
}

CFrustum::~CFrustum()
{
	if (FALSE == m_isClone)
	{
		SAFE_DELETE(m_pFrustumPlane);
	}
}

HRESULT CFrustum::Init_Frustum()
{
	m_vPoints[0] = XMVectorSet(-1.f, 1.f, 0.f, 0.f);
	m_vPoints[1] = XMVectorSet(1.f, 1.f, 0.f, 0.f);
	m_vPoints[2] = XMVectorSet(1.f, -1.f, 0.f, 0.f);
	m_vPoints[3] = XMVectorSet(-1.f, -1.f, 0.f, 0.f);

	m_vPoints[4] = XMVectorSet(-1.f, 1.f, 1.f, 0.f);
	m_vPoints[5] = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	m_vPoints[6] = XMVectorSet(1.f, -1.f, 1.f, 0.f);
	m_vPoints[7] = XMVectorSet(-1.f, -1.f, 1.f, 0.f);

	return S_OK;
}

void CFrustum::Update_Frustum(XMMATRIX* p_matView, XMMATRIX* p_matProj)
{
	XMMATRIX matInvVP = XMMatrixInverse(nullptr, *p_matProj) * XMMatrixInverse(nullptr, *p_matView);

	for (int i = 0; i < 8; ++i)
	{
		m_vWorldPoints[i] = XMVector3TransformCoord(m_vPoints[i], matInvVP);
	}

	// right
	m_pFrustumPlane[0] = XMPlaneFromPoints(m_vWorldPoints[0], m_vWorldPoints[4], m_vWorldPoints[7]);
	// left
	m_pFrustumPlane[1] = XMPlaneFromPoints(m_vWorldPoints[1], m_vWorldPoints[2], m_vWorldPoints[6]);
	// top
	m_pFrustumPlane[2] = XMPlaneFromPoints(m_vWorldPoints[0], m_vWorldPoints[1], m_vWorldPoints[5]);
	// bot
	m_pFrustumPlane[3] = XMPlaneFromPoints(m_vWorldPoints[7], m_vWorldPoints[6], m_vWorldPoints[2]);
	// near
	m_pFrustumPlane[4] = XMPlaneFromPoints(m_vWorldPoints[2], m_vWorldPoints[1], m_vWorldPoints[0]);
	// far
	m_pFrustumPlane[5] = XMPlaneFromPoints(m_vWorldPoints[4], m_vWorldPoints[5], m_vWorldPoints[6]);
}

BOOL CFrustum::Compute_CullingObject(XMVECTOR p_pos, float p_radius)
{
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR dot = XMPlaneDotCoord(m_pFrustumPlane[i], p_pos);

		if (XMVectorGetX(dot) < -p_radius)
		{
			return FALSE;
		}
	}

	return TRUE;
}

CFrustum* CFrustum::Create_Frustum(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
{
	CFrustum* pInstance = new CFrustum(p_Device, p_Context);

	if (FAILED(pInstance->Init_Frustum()))
	{
		MessageBox(g_hWnd, L"CFrustum Create Failed!!", 0, 0);
		SAFE_DELETE(pInstance);
		return nullptr;
	}

	return pInstance;
}
