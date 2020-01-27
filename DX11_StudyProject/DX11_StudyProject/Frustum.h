#pragma once

#include "Component.h"

class CFrustum : public CComponent
{
private:
	explicit CFrustum(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CFrustum(const CFrustum& rhs);

public:
	virtual ~CFrustum();

public:
	inline virtual CComponent* Clone() { return new CFrustum(*this); }

public:
	HRESULT Init_Frustum();
	void Update_Frustum(XMMATRIX* p_matView, XMMATRIX* p_matProj);
	BOOL Compute_CullingObject(XMVECTOR p_pos, float p_radius); // TRUE = visable FALSE = Invisable

public:
	static CFrustum* Create_Frustum(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);

private:
	XMVECTOR m_vPoints[8];
	XMVECTOR m_vWorldPoints[8];
	XMVECTOR* m_pFrustumPlane;
};