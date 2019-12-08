#pragma once

#include "Mesh.h"

class CFigureMesh : public CMesh
{
public:
	explicit CFigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, UINT p_dwType = 0);
	CFigureMesh(const CFigureMesh& rhs);
	virtual ~CFigureMesh();

public:
	inline virtual CComponent* Clone() { return new CFigureMesh(*this); }
public:
	virtual void Init_Mesh();

private:
	UINT		m_dwType;
};

