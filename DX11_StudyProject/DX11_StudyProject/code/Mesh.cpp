#include "Mesh.h"

CMesh::CMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_dwStride(0)
	, m_dwOffset(0)
{

}

CMesh::~CMesh()
{
	m_pVB->Release();
	m_pIB->Release();
}
