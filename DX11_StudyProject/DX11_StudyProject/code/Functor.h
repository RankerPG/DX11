#pragma once
#include <DirectXMath.h>

using namespace DirectX;

template<typename T>
void SAFE_DELETE(T& p_data)
{
	if (nullptr != p_data)
	{
		delete p_data;
		p_data = nullptr;
	}
}

template<typename T>
void SAFE_DELETE_ARRAY(T& p_data)
{
	if (nullptr != p_data)
	{
		delete[] p_data;
		p_data = nullptr;
	}
}

template<typename T>
void SAFE_RELEASE(T& p_data)
{
	if (nullptr != p_data)
	{
		p_data->Release();
		p_data = nullptr;
	}
}

static XMMATRIX InverseTranspose(CXMMATRIX M)
{
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}