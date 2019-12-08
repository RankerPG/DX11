#include "Transform.h"

CTransform::CTransform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_vScale(XMVectorSet(1.f, 1.f, 1.f, 0.f))
	, m_vRot(XMVectorZero())
	, m_vTrans(XMVectorZero())
	, m_matScale(XMMatrixIdentity())
	, m_matRot(XMMatrixIdentity())
	, m_matTrans(XMMatrixIdentity())
	, m_matWorld(XMMatrixIdentity())
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs.m_pDevice, rhs.m_pContext)
	, m_vScale(rhs.m_vScale)
	, m_vRot(rhs.m_vRot)
	, m_vTrans(rhs.m_vTrans)
	, m_matWorld(rhs.m_matWorld)
{

}

CTransform::~CTransform()
{
}

void CTransform::Update_Transform()
{
	m_matScale = XMMatrixScalingFromVector(m_vScale);
	m_matRot = XMMatrixRotationRollPitchYawFromVector(m_vRot);
	m_matTrans = XMMatrixTranslationFromVector(m_vTrans);

	m_matWorld = m_matScale * m_matRot * m_matTrans;
}

CTransform* CTransform::Create_Transform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
{
	CTransform* pInstance = new CTransform(p_Device, p_Context);

	return pInstance;
}
