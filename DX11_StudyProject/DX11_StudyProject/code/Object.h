#pragma once

#include "framework.h"

class CComponent;

class CObject
{
protected:
	typedef unordered_map<LPCSTR, shared_ptr<CComponent>> COMHASHMAP;

public:
	explicit CObject(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	virtual ~CObject();

public:
	virtual void Init() = 0;
	//void FixedUpdate(float p_deltaTime); 추후 추가
	virtual void Update(float p_deltaTime) = 0;
	//void LastUpdate(float p_deltaTime); 추후 추가
	virtual void Render(XMMATRIX* p_matReflect = nullptr) = 0;

private:
	virtual void Release();

protected:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

	COMHASHMAP*				m_pMapComponent;
};

