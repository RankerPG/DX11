#pragma once

#include "framework.h"

class CComponent abstract
{
public:
	explicit CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComponent();

protected:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
};

