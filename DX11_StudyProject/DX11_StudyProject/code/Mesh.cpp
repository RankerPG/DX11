#include "Mesh.h"

CMesh::CMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_dwStride(0)
	, m_dwOffset(0)
	, m_dwIdxCnt(0)
{

}

CMesh::CMesh(const CMesh& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_dwStride(rhs.m_dwStride)
	, m_dwOffset(rhs.m_dwOffset)
	, m_dwIdxCnt(rhs.m_dwIdxCnt)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
}

CMesh::~CMesh()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

void CMesh::Draw_Mesh()
{
	m_pContext->DrawIndexed(m_dwIdxCnt, 0, 0);
}
