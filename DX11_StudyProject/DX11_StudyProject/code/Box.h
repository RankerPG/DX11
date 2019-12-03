#pragma once

#include "framework.h"

class CMesh;
class CTransform;
class CShader;

class CBox
{
public:
	CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	~CBox();

public:
	void Init();
	void Update(float p_deltaTime);
	void Render();

private:
	void Release();

private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext*			m_pContext;

	shared_ptr<CMesh>				m_pMesh;
	shared_ptr<CTransform>			m_pTransform;
	shared_ptr<CShader>				m_pShader;
};

