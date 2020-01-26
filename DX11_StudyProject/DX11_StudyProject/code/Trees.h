#pragma once

#include "Object.h"

class CMesh;
class CTransform;
class CShader;
class CTexture;

class CTrees : public CObject
{
public:
	CTrees(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	~CTrees();

public:
	virtual void Init();
	virtual void Update(float p_deltaTime);
	virtual void Render(XMMATRIX* p_matAdd = nullptr, BOOL p_isUseMtrl = TRUE);

private:
	virtual void Release();

private:
	CTransform*		m_pTransform;
	CShader*		m_pShader;
	CTexture*		m_pTexture;

private: //constant buffer
	ID3D11Buffer*	m_pCB;
	ID3D11Buffer*	m_pCBMtrl;

	TRANSMATRIX		m_mat;
	MATERIAL		m_mtrl;
	XMFLOAT4X4		m_matViewProj;

	XMFLOAT4X4		m_matArrWorld[1000];
	ID3D11Buffer*	m_pVB;
	ID3D11Buffer*	m_pIB;
	UINT			m_iIdxCnt;
	ID3D11Buffer*	m_pInstanceVB;
};

