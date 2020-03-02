#pragma once

#include "Particle.h"

class CParticle_Rain abstract : public CParticle
{
public:
	explicit CParticle_Rain(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CParticle_Rain(const CParticle_Rain& rhs);
	virtual ~CParticle_Rain();

public:
	virtual void Init_Particle();
};