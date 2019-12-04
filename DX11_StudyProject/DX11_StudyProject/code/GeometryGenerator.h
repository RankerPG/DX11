#pragma once

#include "Component.h"

class CGeometryGenerator : public CComponent
{
public:
	struct VertexData
	{
		VertexData() {}
		VertexData(const VertexData& rhs)
			: pos(rhs.pos)
			, nrm(rhs.nrm)
			, tangent(rhs.tangent)
			, uv(rhs.uv) {}
		VertexData(float px, float py, float pz, float nx, float ny, float nz
			, float tx, float ty, float tz, float u, float v)
			: pos(px, py, pz, 1.f)
			, nrm(nx, ny, nz)
			, tangent(tx, ty, tz)
			, uv(u, v) {}

		XMFLOAT4 pos;
		XMFLOAT3 nrm;
		XMFLOAT3 tangent;
		XMFLOAT2 uv;
	};

	struct MeshData
	{
		vector<VertexData> Vertices;
		vector<UINT> Indices;
	};

public:
	explicit CGeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	virtual ~CGeometryGenerator();

public:
	virtual CComponent* Clone() { return nullptr; }

public:
	void Create_Cube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_Terrain(float p_width, float p_depth, UINT m, UINT n, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_Sphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);

private:
	float Get_Height(float p_x, float p_z) { return 0.3f * (p_z * sinf(0.1f * p_x) + p_x * cosf(0.1f * p_z)); }
};

