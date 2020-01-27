#pragma once

#include "Object.h"

class CInput;

class CCamera : public CObject
{
public:
	CCamera(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	~CCamera();

public:
	inline const XMVECTOR& Get_ViewPos() { return m_vPos; }

public:
	virtual void Init();
	virtual void Update(float p_deltaTime);
	virtual void LastUpdate(float p_deltaTime);
	virtual void Render(XMMATRIX* p_matAdd = nullptr, BOOL p_isUseMtrl = TRUE);

private:
	virtual void Release();

private:
	XMVECTOR m_vPos;
	XMVECTOR m_vTarget;
	XMVECTOR m_vUp;
	
	XMMATRIX m_matView;

	CInput* m_pInput;

	BOOL	m_bCameraFix;
};

