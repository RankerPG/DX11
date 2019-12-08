#pragma once

#include "Object.h"

class CMesh;
class CTransform;
class CShader;
class CTexture;

class CBox : public CObject
{
public:
	CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	~CBox();

public:
	virtual void Init();
	virtual void Update(float p_deltaTime);
	virtual void Render();

private:
	virtual void Release();

private:
	CMesh*			m_pMesh;
	CTransform*		m_pTransform;
	CShader*		m_pShader;
	CTexture*		m_pTexture;

private: //constant buffer
	ID3D11Buffer*	m_pCB;
	ID3D11Buffer*	m_pCBMtrl;

	TRANSMATRIX		m_mat;
	MATERIAL		m_mtrl;
};