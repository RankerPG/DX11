#include "FigureMesh.h"
#include "struct.h"
#include "Geometrygenerator.h"

CFigureMesh::CFigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, UINT p_dwType)
	: CMesh(p_Device, p_Context)
	, m_dwType(p_dwType)
{
}

CFigureMesh::CFigureMesh(const CFigureMesh& rhs)
	: CMesh(rhs)
	, m_dwType(rhs.m_dwType)
{
}

CFigureMesh::~CFigureMesh()
{
}

void CFigureMesh::Init_Mesh()
{
	m_dwStride = sizeof(NORMALVERTEX);
	m_dwOffset = 0;

	CGeometryGenerator generator(m_pDevice, m_pContext);
	
	switch (m_dwType)
	{
	case 0:
		generator.Create_Cube(1.f, 1.f, 1.f, &m_pVB, &m_pIB, &m_dwIdxCnt);
		break;
	case 1:
		generator.Create_Sphere(1.f, 40, 20, &m_pVB, &m_pIB, &m_dwIdxCnt);
		break;
	case 2:
		generator.Create_Terrain(128.f, 128.f, 129, 129, &m_pVB, &m_pIB, &m_dwIdxCnt);
		break;
	}
}
