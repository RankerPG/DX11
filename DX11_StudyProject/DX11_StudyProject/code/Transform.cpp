#include "Transform.h"

CTransform::CTransform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_vScale(XMVectorSet(1.f, 1.f, 1.f, 0.f))
	, m_vRot(XMVectorZero())
	, m_vTrans(XMVectorZero())
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
	XMMATRIX matScale, matRot, matTrans;
	
	matScale = XMMatrixScalingFromVector(m_vScale);
	matRot = XMMatrixRotationRollPitchYawFromVector(m_vRot);
	matTrans = XMMatrixTranslationFromVector(m_vTrans);

	m_matWorld = matScale * matRot * matTrans;
}
