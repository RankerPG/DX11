#pragma once

#include "Component.h"

class CTexture;

class CParticle abstract : public CComponent
{
public:
	explicit CParticle(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CParticle(const CParticle& rhs);
	virtual ~CParticle();

public:
	virtual void Init_Particle() = 0;

protected:
	void Create_Buffer();

	void Render_Particle();

protected:
	ID3D11Buffer*			m_pInitVB;
	ID3D11Buffer*			m_pDrawVB;
	ID3D11Buffer*			m_pStreamOutVB;

protected:
	UINT					m_dwMaxParticles;
	BOOL					m_isFirstRun;

	float					m_fGameTime;
	float					m_fTimeStep;
	float					m_fAge;

	XMFLOAT3				m_vecEyePosW;
	XMFLOAT3				m_vecEmitPosW;
	XMFLOAT3				m_vecEmitDirW;
};