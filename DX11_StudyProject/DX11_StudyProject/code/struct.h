#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;

struct VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT4 col;
};

struct NORMALVERTEX
{
	XMFLOAT4 pos;
	XMFLOAT3 nrm;
};

struct TEXVERTEX
{
	XMFLOAT4 pos;
	XMFLOAT3 nrm;
	XMFLOAT2 uv;
};

struct TRANSMATRIX
{
	XMFLOAT4X4 matWorld;
	XMFLOAT4X4 matWorldRT;
	XMFLOAT4X4 matWVP;
};

struct LIGHT
{
	XMFLOAT3A direction;
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
};

struct POINTLIGHT
{
	XMFLOAT3 position;
	float	 Range;
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
};

struct MATERIAL
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular; // w = power
};

struct PERFRAME
{
	XMFLOAT3A viewPos;
};