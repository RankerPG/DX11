#pragma once

#include "framework.h"

class CComponent;
class CFrustum;

class CObject
{
protected:
	typedef unordered_map<LPCSTR, shared_ptr<CComponent>> COMHASHMAP;

public:
	explicit CObject(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap);
	virtual ~CObject();

public:
	inline const float& Get_Radius() { return m_fRadius; }
	
	inline const bool& Get_Visible() { return m_isVisible; }

	inline void Set_Visible(bool p_isVisible) { m_isVisible = p_isVisible; }

public:
	virtual void Init() = 0;
	//void FixedUpdate(float p_deltaTime); 추후 추가
	virtual void Update(float p_deltaTime) = 0;
	virtual void LastUpdate(float p_deltaTime) = 0;
	virtual void Render(XMMATRIX* p_matAdd = nullptr, BOOL p_isUseMtrl = TRUE) = 0;

private:
	virtual void Release();

protected:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

	COMHASHMAP*				m_pMapComponent;

	float					m_fRadius;
	bool					m_isVisible;
};

