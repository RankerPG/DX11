#pragma once

#include "Component.h"

class CTransform : public CComponent
{
private:
	explicit CTransform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CTransform(const CTransform& rhs);

public:
	virtual ~CTransform();

public:
	inline virtual CComponent* Clone() { return new CTransform(*this); }

public:
	inline const XMVECTOR& Get_Scale() const { return m_vScale; }
	inline const XMVECTOR& Get_Rot() const { return m_vRot; }
	inline const XMVECTOR& Get_Trans() const { return m_vTrans; }
	inline const XMMATRIX& Get_Tex() const { return m_matTex; }
	inline const XMMATRIX& Get_World() const { return m_matWorld; }

	inline void Set_Scale(XMVECTOR p_Scale) { m_vScale = p_Scale; }
	inline void Set_Rotation(XMVECTOR p_Rot) { m_vRot = p_Rot; }
	inline void Set_Trans(XMVECTOR p_Trans) { m_vTrans = p_Trans; }
	inline void Set_TexScale(XMVECTOR p_TexScale) { m_vTexScale = p_TexScale; }
	inline void Set_TexRot(XMVECTOR p_TexRot) { m_vTexRot = p_TexRot; }
	inline void Set_TexTrans(XMVECTOR p_TexTrans) { m_vTexTrans = p_TexTrans; }

	inline void Acc_Scale(XMVECTOR p_Scale) { m_vScale += p_Scale; }
	inline void Acc_Rotation(XMVECTOR p_Rot) { m_vRot += p_Rot; }
	inline void Acc_Trans(XMVECTOR p_Trans) { m_vTrans += p_Trans; }
	inline void Acc_TexScale(XMVECTOR p_TexScale) { m_vTexScale += p_TexScale; }
	inline void Acc_TexRot(XMVECTOR p_TexRot) { m_vTexRot += p_TexRot; }
	inline void Acc_TexTrans(XMVECTOR p_TexTrans) { m_vTexTrans += p_TexTrans; }

	inline void Set_MatRot(XMMATRIX p_matRot) { m_matRot = p_matRot; }

public:
	void Update_Transform();

public:
	static CTransform* Create_Transform(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);

private:
	XMVECTOR	m_vScale;
	XMVECTOR	m_vRot;
	XMVECTOR	m_vTrans;
	XMVECTOR	m_vTexScale;
	XMVECTOR	m_vTexRot;
	XMVECTOR	m_vTexTrans;
				
	XMMATRIX	m_matScale;
	XMMATRIX	m_matRot;
	XMMATRIX	m_matTrans;
	XMMATRIX	m_matTex;
	XMMATRIX	m_matWorld;
};

