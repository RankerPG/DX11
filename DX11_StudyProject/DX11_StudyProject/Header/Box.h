#pragma once

#include "framework.h"

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT4 col;
};

class CBox
{
public:
	CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	~CBox();

public:
	void Init();
	void Update();
	void Render();

private:
	void Release();

private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;

	ID3D11Buffer*					m_pVB;
	ID3D11Buffer*					m_pIB;

	ID3D11VertexShader*				m_pVS;
	ID3D11PixelShader*				m_pPS;
	ID3D11Buffer*					m_pCB;
	ID3D11InputLayout*				m_inputLayout;

	XMFLOAT4X4						mWorld;
	XMFLOAT4X4						mView;
	XMFLOAT4X4						mProj;
	
	XMMATRIX matWVP;
};

