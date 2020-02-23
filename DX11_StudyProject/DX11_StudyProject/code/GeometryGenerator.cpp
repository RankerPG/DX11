#include "GeometryGenerator.h"
#include "struct.h"


CGeometryGenerator::CGeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
{
}

CGeometryGenerator::~CGeometryGenerator()
{
}

void CGeometryGenerator::Create_Cube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;
	
	Get_CubeData(p_width, p_height, p_depth, data);

	NORMALVERTEX vertices[24];

	for (int i = 0; i < 24; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(NORMALVERTEX) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	
	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		SAFE_RELEASE(*p_VB);
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = 36;
}

void CGeometryGenerator::Create_Terrain(float p_width, float p_depth, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_TerrainData(p_width, p_depth, 1.f, n, m, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<NORMALVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(NORMALVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_Sphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_SphereData(p_radius, p_sliceCount, p_stackCount, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<NORMALVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(NORMALVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_TexCube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_CubeData(p_width, p_height, p_depth, data);

	TEXVERTEX vertices[24];

	for (int i = 0; i < 24; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].uv = data.Vertices[i].uv;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TEXVERTEX) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = 36;
}

void CGeometryGenerator::Create_TexTerrain(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_TerrainData(p_width, p_depth, p_maxUV, n, m, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TEXVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TEXVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_TexSphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_SphereData(p_radius, p_sliceCount, p_stackCount, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TEXVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TEXVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_TexQuad(ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_QuadData(data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TEXVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TEXVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_NormalCube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_CubeData(p_width, p_height, p_depth, data);

	TBNVERTEX vertices[24];

	for (int i = 0; i < 24; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].tan = data.Vertices[i].tangent;
		vertices[i].uv = data.Vertices[i].uv;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TBNVERTEX) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = vertices;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = 36;
}

void CGeometryGenerator::Create_NormalTerrain(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_TerrainData(p_width, p_depth, p_maxUV, n, m, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TBNVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].tan = data.Vertices[i].tangent;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TBNVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_NormalSphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_SphereData(p_radius, p_sliceCount, p_stackCount, data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TBNVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].tan = data.Vertices[i].tangent;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TBNVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_NormalQuad(ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_QuadData(data);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TBNVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].tan = data.Vertices[i].tangent;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TBNVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Create_NormalTileQuad(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	Get_TerrainData(p_width, p_depth, p_maxUV, n, m, data, FALSE);

	UINT vtxCnt = (UINT)data.Vertices.size();

	vector<TBNVERTEX> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].nrm = data.Vertices[i].nrm;
		vertices[i].tan = data.Vertices[i].tangent;
		vertices[i].uv = data.Vertices[i].uv;
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(TBNVERTEX) * vtxCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &vertices[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	UINT idxCnt = (UINT)data.Indices.size();

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * idxCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = idxCnt;
}

void CGeometryGenerator::Get_CubeData(float p_width, float p_height, float p_depth, MeshData& p_data)
{
	p_data.Vertices.resize(24);

	float w2 = 0.5f * p_width;
	float h2 = 0.5f * p_height;
	float d2 = 0.5f * p_depth;

	// Fill in the front face vertex data.
	p_data.Vertices[0] = VertexData(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	p_data.Vertices[1] = VertexData(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	p_data.Vertices[2] = VertexData(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	p_data.Vertices[3] = VertexData(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	p_data.Vertices[4] = VertexData(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	p_data.Vertices[5] = VertexData(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	p_data.Vertices[6] = VertexData(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	p_data.Vertices[7] = VertexData(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	p_data.Vertices[8] = VertexData(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	p_data.Vertices[9] = VertexData(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	p_data.Vertices[10] = VertexData(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	p_data.Vertices[11] = VertexData(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	p_data.Vertices[12] = VertexData(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	p_data.Vertices[13] = VertexData(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	p_data.Vertices[14] = VertexData(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	p_data.Vertices[15] = VertexData(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	p_data.Vertices[16] = VertexData(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	p_data.Vertices[17] = VertexData(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	p_data.Vertices[18] = VertexData(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	p_data.Vertices[19] = VertexData(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	p_data.Vertices[20] = VertexData(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	p_data.Vertices[21] = VertexData(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	p_data.Vertices[22] = VertexData(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	p_data.Vertices[23] = VertexData(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	p_data.Indices.assign(&i[0], &i[36]);
}

void CGeometryGenerator::Get_TerrainData(float p_width, float p_depth, float p_maxUV, UINT m, UINT n, MeshData& p_data, BOOL p_isTerrain)
{
	UINT vtxCnt = n * m;
	UINT faceCnt = (n - 1) * (m - 1) * 2;

	float startX = p_width * 0.5f;
	float startZ = p_depth * 0.5f;

	float dx = p_width / (n - 1);
	float dz = p_depth / (m - 1);

	float du = p_maxUV / (n - 1);
	float dv = p_maxUV / (m - 1);

	p_data.Vertices.resize(vtxCnt);

	for (UINT z = 0; z < m; ++z)
	{
		float posZ = startZ - z * dz;

		for (UINT x = 0; x < n; ++x)
		{
			float posX = x * dx - startX;

			float u = du * x;
			float v = dv * z;

			float posY = 0.f;

			if (TRUE == p_isTerrain && -10.f < posX && 10.f > posX && -10.f < posZ && 10.f > posZ)
				posY = -2.f;

			p_data.Vertices[x + z * n].pos = XMFLOAT4(posX, posY, posZ, 1.f);
			p_data.Vertices[x + z * n].nrm = XMFLOAT3(0.f, 0.f, 0.f);
			p_data.Vertices[x + z * n].tangent = XMFLOAT3(1.f, 0.f, 0.f);
			p_data.Vertices[x + z * n].uv = XMFLOAT2(u, v);
		}
	}

	vector<UINT> vecNrmCnt(vtxCnt);

	for (UINT z = 0; z < m - 1; ++z)
	{
		for (UINT x = 0; x < n - 1; ++x)
		{
			UINT index = z * n + x;
			XMVECTOR p0 = XMLoadFloat4(&p_data.Vertices[index].pos);
			XMVECTOR p1 = XMLoadFloat4(&p_data.Vertices[index+1].pos);
			XMVECTOR p2 = XMLoadFloat4(&p_data.Vertices[index+n].pos);
			XMVECTOR p3 = XMLoadFloat4(&p_data.Vertices[index+n+1].pos);

			XMVECTOR v1 = XMVector3Normalize(p1 - p0);
			XMVECTOR v2 = XMVector3Normalize(p2 - p0);

			XMVECTOR nrm = XMVector3Cross(v1, v2);
			XMFLOAT4 vNrm;
			XMStoreFloat4(&vNrm, nrm);

			p_data.Vertices[index].nrm.x += vNrm.x;
			p_data.Vertices[index].nrm.y += vNrm.y;
			p_data.Vertices[index].nrm.z += vNrm.z;

			p_data.Vertices[index + 1].nrm.x += vNrm.x;
			p_data.Vertices[index + 1].nrm.y += vNrm.y;
			p_data.Vertices[index + 1].nrm.z += vNrm.z;

			p_data.Vertices[index + n].nrm.x += vNrm.x;
			p_data.Vertices[index + n].nrm.y += vNrm.y;
			p_data.Vertices[index + n].nrm.z += vNrm.z;

			//
			v1 = XMVector3Normalize(p1 - p3);
			v2 = XMVector3Normalize(p2 - p3);

			nrm = XMVector3Cross(v2, v1);
			XMStoreFloat4(&vNrm, nrm);

			p_data.Vertices[index + 1].nrm.x += vNrm.x;
			p_data.Vertices[index + 1].nrm.y += vNrm.y;
			p_data.Vertices[index + 1].nrm.z += vNrm.z;

			p_data.Vertices[index + n].nrm.x += vNrm.x;
			p_data.Vertices[index + n].nrm.y += vNrm.y;
			p_data.Vertices[index + n].nrm.z += vNrm.z;

			p_data.Vertices[index + n + 1].nrm.x += vNrm.x;
			p_data.Vertices[index + n + 1].nrm.y += vNrm.y;
			p_data.Vertices[index + n + 1].nrm.z += vNrm.z;

			++vecNrmCnt[index];
			vecNrmCnt[index + 1] += 2;
			vecNrmCnt[index + n] += 2;
			++vecNrmCnt[index + n + 1];
		}
	}

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		p_data.Vertices[i].nrm.x /= vecNrmCnt[i];
		p_data.Vertices[i].nrm.y /= vecNrmCnt[i];
		p_data.Vertices[i].nrm.z /= vecNrmCnt[i];
	}

	p_data.Indices.resize(faceCnt * 3);

	UINT index = 0;

	for (UINT z = 0; z < m - 1; ++z)
	{
		for (UINT x = 0; x < n - 1; ++x)
		{
			// 좌상단
			p_data.Indices[index] = z * n + x;
			p_data.Indices[index + 1] = z * n + x + 1;
			p_data.Indices[index + 2] = (z + 1) * n + x;

			// 우하단
			p_data.Indices[index + 3] = (z + 1) * n + x;
			p_data.Indices[index + 4] = z * n + x + 1;
			p_data.Indices[index + 5] = (z + 1) * n + x + 1;

			index += 6;
		}
	}
}

void CGeometryGenerator::Get_SphereData(float p_radius, UINT p_sliceCount, UINT p_stackCount, MeshData& p_data)
{
	VertexData topVertex(0.0f, +p_radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	VertexData bottomVertex(0.0f, -p_radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	p_data.Vertices.push_back(topVertex);

	float phiStep = XM_PI / p_stackCount;
	float thetaStep = 2.0f * XM_PI / p_sliceCount;

	for (UINT i = 1; i <= p_stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= p_sliceCount; ++j)
		{
			float theta = j * thetaStep;

			VertexData v;

			v.pos.x = p_radius * sinf(phi) * cosf(theta);
			v.pos.y = p_radius * cosf(phi);
			v.pos.z = p_radius * sinf(phi) * sinf(theta);
			v.pos.w = 1.f;

			v.tangent.x = -p_radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = +p_radius * sinf(phi) * cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.tangent);
			XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat4(&v.pos);
			XMStoreFloat3(&v.nrm, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;

			p_data.Vertices.push_back(v);
		}
	}

	p_data.Vertices.push_back(bottomVertex);

	for (UINT i = 1; i <= p_sliceCount; ++i)
	{
		p_data.Indices.push_back(0);
		p_data.Indices.push_back(i + 1);
		p_data.Indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = p_sliceCount + 1;
	for (UINT i = 0; i < p_stackCount - 2; ++i)
	{
		for (UINT j = 0; j < p_sliceCount; ++j)
		{
			p_data.Indices.push_back(baseIndex + i * ringVertexCount + j);
			p_data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			p_data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			p_data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			p_data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			p_data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (UINT)p_data.Vertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < p_sliceCount; ++i)
	{
		p_data.Indices.push_back(southPoleIndex);
		p_data.Indices.push_back(baseIndex + i);
		p_data.Indices.push_back(baseIndex + i + 1);
	}
}

void CGeometryGenerator::Get_QuadData(MeshData& p_data)
{
	p_data.Vertices.resize(4);
	p_data.Indices.resize(6);

	p_data.Vertices[0] = VertexData(
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f);

	p_data.Vertices[1] = VertexData(
		-0.5f, +0.5f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f);

	p_data.Vertices[2] = VertexData(
		+0.5f, +0.5f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f);

	p_data.Vertices[3] = VertexData(
		+0.5f, -0.5f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f);

	p_data.Indices[0] = 0;
	p_data.Indices[1] = 1;
	p_data.Indices[2] = 2;

	p_data.Indices[3] = 0;
	p_data.Indices[4] = 2;
	p_data.Indices[5] = 3;
}

CGeometryGenerator* CGeometryGenerator::Create_GeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
{
	CGeometryGenerator* pInstance = new CGeometryGenerator(p_Device, p_Context);

	return pInstance;
}
