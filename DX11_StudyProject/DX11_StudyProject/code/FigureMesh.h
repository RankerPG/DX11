#pragma once

#include "Mesh.h"

class CFigureMesh : public CMesh
{
private:
	explicit CFigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, UINT p_dwType);
	CFigureMesh(const CFigureMesh& rhs);

public:
	virtual ~CFigureMesh();

public:
	inline virtual CComponent* Clone() { return new CFigureMesh(*this); }
public:
	virtual void Init_Mesh();

public:
	static CFigureMesh* Create_FigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, UINT p_dwType = 0);

private:
	UINT		m_dwType;
};

