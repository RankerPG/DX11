#include "GeometryGenerator.h"
#include "struct.h"


CGeometryGenerator::CGeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
{
}

CGeometryGenerator::~CGeometryGenerator()
{
}

void CGeometryGenerator::Create_Cube(ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
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

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_VB)))
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

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = 36;

	//MeshData data;
	//data.Vertices.resize(8);

	//data.Vertices[0].pos = XMFLOAT3(-1.f, -1.f, -1.f);
	//data.Vertices[0].nrm = XMFLOAT3(0.f, 0.f, -1.f);
	//data.Vertices[0].uv = XMFLOAT2(0.f, 1.f);

	//data.Vertices[1].pos = XMFLOAT3(-1.f, +1.f, -1.f);
	//data.Vertices[1].nrm = XMFLOAT3(0.f, 0.f, -1.f);
	//data.Vertices[1].uv = XMFLOAT2(0.f, 0.f);

	//data.Vertices[2].pos = XMFLOAT3(+1.f, +1.f, -1.f);
	//data.Vertices[2].nrm = XMFLOAT3(0.f, 0.f, -1.f);
	//data.Vertices[2].uv = XMFLOAT2(1.f, 0.f);

	//data.Vertices[3].pos = XMFLOAT3(+1.f, -1.f, -1.f);
	//data.Vertices[3].nrm = XMFLOAT3(0.f, 0.f, -1.f);
	//data.Vertices[3].uv = XMFLOAT2(1.f, 1.f);

	//data.Vertices[4].pos = XMFLOAT3(-1.f, -1.f, +1.f);
	//data.Vertices[4].nrm = XMFLOAT3(0.f, 0.f, 1.f);
	//data.Vertices[4].uv = XMFLOAT2(1.f, 1.f);

	//data.Vertices[5].pos = XMFLOAT3(-1.f, +1.f, +1.f);
	//data.Vertices[5].nrm = XMFLOAT3(0.f, 0.f, 1.f);
	//data.Vertices[5].uv = XMFLOAT2(1.f, 0.f);

	//data.Vertices[6].pos = XMFLOAT3(+1.f, +1.f, +1.f);
	//data.Vertices[6].nrm = XMFLOAT3(0.f, 0.f, 1.f);
	//data.Vertices[6].uv = XMFLOAT2(0.f, 0.f);

	//data.Vertices[7].pos = XMFLOAT3(+1.f, -1.f, +1.f);
	//data.Vertices[7].nrm = XMFLOAT3(0.f, 0.f, 1.f);
	//data.Vertices[7].uv = XMFLOAT2(0.f, 1.f);
}

void CGeometryGenerator::Create_Terrain(float p_width, float p_depth, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	MeshData data;

	UINT vtxCnt = n * m;
	UINT faceCnt = (n - 1) * (m - 1) * 2;

	float startX = p_width * 0.5f;
	float startZ = p_depth * 0.5f;

	float dx = p_width / (n - 1);
	float dz = p_depth / (m - 1);

	float du = 1.f / (n - 1);
	float dv = 1.f / (m - 1);

	data.Vertices.resize(vtxCnt);

	for (int z = 0; z < m; ++z)
	{
		float posZ = startZ - z * dz;

		for (int x = 0; x < n; ++x)
		{
			float posX = x * dx - startX;

			float u = du * x;
			float v = dv * z;

			data.Vertices[x + z * n].pos = XMFLOAT4(posX, 0.f, posZ, 1.f);
			data.Vertices[x + z * n].nrm = XMFLOAT3(0.f, 1.f, 0.f);
			data.Vertices[x + z * n].tangent = XMFLOAT3(1.f, 0.f, 0.f);
			data.Vertices[x + z * n].uv = XMFLOAT2(du, dv);
		}
	}

	data.Indices.resize(faceCnt * 3);

	UINT index = 0;

	for (int z = 0; z < m-1; ++z)
	{
		for (int x = 0; x < n-1; ++x)
		{
			// 좌상단
			data.Indices[index] = z * n + x;
			data.Indices[index+1] = z * n + x + 1;
			data.Indices[index+2] = (z + 1) * n + x;
					
			// 우하단
			data.Indices[index+3] = (z + 1) * n + x;
			data.Indices[index+4] = z * n + x + 1;
			data.Indices[index+5] = (z + 1) * n + x + 1;

			index += 6;
		}
	}

	vector<Vertex> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		XMFLOAT4 p = data.Vertices[i].pos;
		//p.y = Get_Height(p.x, p.z);
		vertices[i].pos = p;

		//if (p.y < -10.f)
		//{
		//	vertices[i].col = XMFLOAT4(1.f, 0.96f, 0.62f, 1.f);
		//}
		//else if (p.y < 5.f)
		//{
		//	vertices[i].col = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.f);
		//}
		//else if (p.y < 12.f)
		//{
		//	vertices[i].col = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.f);
		//}
		//else if (p.y < 20.f)
		//{
		//	vertices[i].col = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.f);
		//}
		//else
		//{
		//	vertices[i].col = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		//}

		vertices[i].col = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * vtxCnt;
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

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(UINT) * faceCnt * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subData.pSysMem = &data.Indices[0];

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, p_IB)))
	{
		(*p_VB)->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	*p_dwIdxCnt = faceCnt * 3;
}

void CGeometryGenerator::Create_Sphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt)
{
	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	VertexData topVertex(0.0f, +p_radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	VertexData bottomVertex(0.0f, -p_radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	
	MeshData data;
	data.Vertices.push_back(topVertex);

	float phiStep = XM_PI / p_stackCount;
	float thetaStep = 2.0f * XM_PI / p_sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= p_stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= p_sliceCount; ++j)
		{
			float theta = j * thetaStep;

			VertexData v;

			// spherical to cartesian
			v.pos.x = p_radius * sinf(phi) * cosf(theta);
			v.pos.y = p_radius * cosf(phi);
			v.pos.z = p_radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.tangent.x = -p_radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = +p_radius * sinf(phi) * cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.tangent);
			XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat4(&v.pos);
			XMStoreFloat3(&v.nrm, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;

			data.Vertices.push_back(v);
		}
	}

	data.Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= p_sliceCount; ++i)
	{
		data.Indices.push_back(0);
		data.Indices.push_back(i + 1);
		data.Indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = p_sliceCount + 1;
	for (UINT i = 0; i < p_stackCount - 2; ++i)
	{
		for (UINT j = 0; j < p_sliceCount; ++j)
		{
			data.Indices.push_back(baseIndex + i * ringVertexCount + j);
			data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = data.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < p_sliceCount; ++i)
	{
		data.Indices.push_back(southPoleIndex);
		data.Indices.push_back(baseIndex + i);
		data.Indices.push_back(baseIndex + i + 1);
	}

	UINT vtxCnt = data.Vertices.size();

	vector<Vertex> vertices(vtxCnt);

	for (UINT i = 0; i < vtxCnt; ++i)
	{
		vertices[i].pos = data.Vertices[i].pos;
		vertices[i].col = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	}

	//
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * vtxCnt;
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

	UINT idxCnt = data.Indices.size();

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
