#include "Camera.h"
#include "Input.h"

CCamera::CCamera(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_vPos(XMVectorZero())
	, m_vTarget(XMVectorZero())
	, m_vUp(XMVectorZero())
	, m_matView(XMMatrixIdentity())
	, m_pInput(nullptr)
	, m_bCameraFix(TRUE)
{
}

CCamera::~CCamera()
{
	m_pInput = nullptr;
}

void CCamera::Init()
{
	m_vPos = XMVectorSet(0.f, 3.f, -15.f, 1.f);
	m_vTarget = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	m_vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_matView = XMMatrixLookAtLH(m_vPos, m_vTarget, m_vUp);

	g_matView = m_matView;

	m_pInput = CInput::Get_Instance();
}

void CCamera::Update(float p_deltaTime)
{
	if (m_pInput->Get_DIKPressState(DIK_LCONTROL))
	{
		m_bCameraFix ^= TRUE;
	}

	if (TRUE == m_bCameraFix)
		return;

	XMVECTOR vRight, vUp, vLook;
	vLook = XMVector3Normalize(m_vTarget - m_vPos);
	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	float fSpeed = p_deltaTime * 10.f;

	if (m_pInput->Get_DIKState(DIK_W))
	{
		m_vPos += vLook * fSpeed;
		m_vTarget += vLook * fSpeed;
	}
	if (m_pInput->Get_DIKState(DIK_S))
	{
		m_vPos -= vLook * fSpeed;
		m_vTarget -= vLook * fSpeed;
	}
	if (m_pInput->Get_DIKState(DIK_A))
	{
		m_vPos -= vRight * fSpeed;
		m_vTarget -= vRight * fSpeed;
	}
	if (m_pInput->Get_DIKState(DIK_D))
	{
		m_vPos += vRight * fSpeed;
		m_vTarget += vRight * fSpeed;
	}

	float fTargetMove = 0.f;

	if (fTargetMove = (float)m_pInput->Get_DIMMoveState(CInput::DIMMS_X))
	{
		fTargetMove *= p_deltaTime;

		XMMATRIX matRot = XMMatrixRotationAxis(vUp, fTargetMove);

		vLook = XMVector3TransformCoord(vLook, matRot);

		m_vTarget = m_vPos + vLook;
	}

	if (fTargetMove = (float)m_pInput->Get_DIMMoveState(CInput::DIMMS_Y))
	{
		fTargetMove *= p_deltaTime;

		XMMATRIX matRot = XMMatrixRotationAxis(vRight, fTargetMove);

		vLook = XMVector3TransformCoord(vLook, matRot);

		m_vTarget = m_vPos + vLook;
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
