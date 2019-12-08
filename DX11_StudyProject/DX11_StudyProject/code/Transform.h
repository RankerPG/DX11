#pragma once

#include "Component.h"

class CTransform : public CComponent
{
public:
	explicit CTransform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	inline virtual CComponent* Clone() { return new CTransform(*this); }

public:
	inline void Set_Scale(XMVECTOR p_Scale) { m_vScale = p_Scale; }
	inline void Set_Rotation(XMVECTOR p_Rot) { m_vRot = p_Rot; }
	inline void Set_Trans(XMVECTOR p_Trans) { m_vTrans = p_Trans; }

	inline void Acc_Scale(XMVECTOR p_Scale) { m_vScale += p_Scale; }
	inline void Acc_Rotation(XMVECTOR p_Rot) { m_vRot += p_Rot; }
	inline void Acc_Trans(XMVECTOR p_Trans) { m_vTrans += p_Trans; }

	inline const XMMATRIX& Get_World() const { return m_matWorld; }

public:
	void Update_Transform();

private:
	XMVECTOR	m_vScale;
	XMVECTOR	m_vRot;
	XMVECTOR	m_vTrans;
				
	XMMATRIX	m_matWorld;
};

