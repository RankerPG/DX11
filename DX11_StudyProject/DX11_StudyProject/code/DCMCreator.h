#pragma once

#include "Component.h"

class CMainFrame;

class CDCMCreator : public CComponent // Dynamic Cube Map Creator
{
private:
	explicit CDCMCreator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);
	CDCMCreator(const CDCMCreator& rhs);

public:
	virtual ~CDCMCreator();

public:
	ID3D11ShaderResourceView** Get_CubeMapSRV() { return &m_pCubeMapSRV; }

public:
	inline virtual CComponent* Clone() { return new CDCMCreator(*this); }

private:
	HRESULT Init_DCM();

public:
	void Update_RenderTarget(XMFLOAT3 p_vPos, CMainFrame* p_mainFrame);

public:
	static CDCMCreator* Create_DCM(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context);

public:
	static UINT g_dwTexSize;

private:
	ID3D11RenderTargetView*			m_pArrCubeMapRTV[6];
	ID3D11ShaderResourceView*		m_pCubeMapSRV;
	ID3D11DepthStencilView*			m_pCubeMapDSV;
	D3D11_VIEWPORT					m_Viewport;

	XMFLOAT3						m_vUps[6];
};

