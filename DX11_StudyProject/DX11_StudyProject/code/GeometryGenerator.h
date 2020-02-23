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

private:
	explicit CGeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);

public:
	virtual ~CGeometryGenerator();

public:
	virtual CComponent* Clone() { return nullptr; }

public:
	void Create_Cube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_Terrain(float p_width, float p_depth, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_Sphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);

	void Create_TexCube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_TexTerrain(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_TexSphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_TexQuad(ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);

	void Create_NormalCube(float p_width, float p_height, float p_depth, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_NormalTerrain(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_NormalSphere(float p_radius, UINT p_sliceCount, UINT p_stackCount, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_NormalQuad(ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);
	void Create_NormalTileQuad(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, ID3D11Buffer** p_VB, ID3D11Buffer** p_IB, UINT* p_dwIdxCnt);

private:
	void Get_CubeData(float p_width, float p_height, float p_depth, MeshData& p_data);
	void Get_TerrainData(float p_width, float p_depth, float p_maxUV, UINT n, UINT m, MeshData& p_data, BOOL p_isTerrain = TRUE);
	void Get_SphereData(float p_radius, UINT p_sliceCount, UINT p_stackCount, MeshData& p_data);
	void Get_QuadData(MeshData& p_data);

public:
	static CGeometryGenerator* Create_GeometryGenerator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
};

