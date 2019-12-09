#pragma once

#include "Object.h"

class CMesh;
class CTransform;
class CShader;

class CVisible : public CObject
{
public:
	CVisible(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	~CVisible();

public:
	const XMVECTOR Get_PointLightPos() const { return m_vTrans; }

public:
	virtual void Init();
	virtual void Update(float p_deltaTime);
	virtual void Render(XMMATRIX* p_matAdd = nullptr, BOOL p_isUseMtrl = TRUE);

private:
	virtual void Release();

private:
	CMesh* m_pMesh;
	CTransform* m_pTransform;
	CShader* m_pShader;

private:
	XMVECTOR		m_vTrans;

	float			m_fAngle;

private: //constant buffer
	ID3D11Buffer*	m_pCB;
	ID3D11Buffer*	m_pCBMtrl;

	XMFLOAT4X4		m_mat;
	MATERIAL		m_mtrl;
};

