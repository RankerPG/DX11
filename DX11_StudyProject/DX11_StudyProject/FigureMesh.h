#pragma once

#include "Mesh.h"

class CFigureMesh : public CMesh
{
public:
	explicit CFigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	virtual ~CFigureMesh();

public:
	virtual void Init_Mesh();
};

