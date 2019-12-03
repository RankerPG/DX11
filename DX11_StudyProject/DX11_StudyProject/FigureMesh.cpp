#include "FigureMesh.h"
#include "struct.h"

CFigureMesh::CFigureMesh(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CMesh(p_Device, p_Context)
{
}

CFigureMesh::~CFigureMesh()
{
}

void CFigureMesh::Init_Mesh()
{
	m_dwStride = sizeof(Vertex);
	m_dwOffset = 0;

	// 버텍스 버퍼 생성
	Vertex vertices[] =
	{
		{ XMFLOAT4(-1.f, -1.f, -1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) },
		{ XMFLOAT4(-1.f, +1.f, -1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, +1.f, -1.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, -1.f, -1.f, 1.f), XMFLOAT4(0.f, 1.f, 0.f, 1.f) },
		{ XMFLOAT4(-1.f, -1.f, +1.f, 1.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f) },
		{ XMFLOAT4(-1.f, +1.f, +1.f, 1.f), XMFLOAT4(1.f, 1.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, +1.f, +1.f, 1.f), XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
		{ XMFLOAT4(+1.f, -1.f, +1.f, 1.f), XMFLOAT4(1.f, 0.f, 1.f, 1.f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, &m_pVB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	// 인덱스 버퍼 생성
	UINT indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = indices;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, &m_pIB)))
	{
		m_pVB->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}
}
