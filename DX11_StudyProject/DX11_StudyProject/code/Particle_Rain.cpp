#include "Particle_Rain.h"

CParticle_Rain::CParticle_Rain(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CParticle(p_Device, p_Context)
{

}

CParticle_Rain::CParticle_Rain(const CParticle_Rain& rhs)
	: CParticle(rhs)
{
}

CParticle_Rain::~CParticle_Rain()
{
}

void CParticle_Rain::Init_Particle()
{
}
