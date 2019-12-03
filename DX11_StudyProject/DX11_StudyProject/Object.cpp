#include "Object.h"

CObject::CObject(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: m_pDevice(p_Device)
	, m_pContext(p_Context)
	, m_pMapComponent(p_hashMap)
{
	m_pDevice->AddRef();
	m_pContext->AddRef();
}

CObject::~CObject()
{
	Release();
}

void CObject::Release()
{
	m_pDevice->Release();
	m_pContext->Release();
}
