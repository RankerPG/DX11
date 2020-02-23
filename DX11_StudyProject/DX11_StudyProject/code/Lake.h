#pragma once

#include "Object.h"

class CMesh;
class CTransform;
class CShader;
class CTexture;
class CFrustum;

class CLake : public CObject
{
public:
	CLake(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	~CLake();

public:
	virtual void Init();
	virtual void Update(float p_deltaTime);
	virtual void LastUpdate(float p_deltaTime);
	virtual void Render(XMMATRIX* p_matAdd = nullptr, BOOL p_isUseMtrl = TRUE);

private:
	virtual void Release();

private:
	CMesh* m_pMesh;
	CTransform* m_pTransform;
	CTransform* m_pTransform_2;
	CShader* m_pShader;
	CTexture* m_pTexture;
	CTexture* m_pTexture_2;
	CTexture* m_pTexture_N;
	CTexture* m_pTexture_N2;
	CFrustum* m_pFrustum;

private: //constant buffer
	ID3D11Buffer*		m_pCB;
	ID3D11Buffer*		m_pCBMtrl;

	TRANSMATRIX_TEX2	m_mat;
	MATERIAL			m_mtrl;

private:
	XMFLOAT2			m_vecTexTrans[2];
};