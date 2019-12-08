#pragma once

#include "framework.h"

class CDevice;
class CInput;
class CTimer;
class CComponent;
class CBox;
class CTerrain;
class CCamera;
class CSphere;
class CShader;
class CVisible;

class CMainFrame
{
	typedef unordered_map<LPCSTR, shared_ptr<CComponent>> COMHASHMAP;

public:
	CMainFrame(CDevice* p_Device);
	~CMainFrame();

public:
	void Init();
	void Update();
	void Render();
	void Release();

private:
	void Calculate_FPS();

private:
	void Create_Device();
	void Create_Components();
	void Create_RasterizerState();
	void Create_Sampler();
	void Create_Object();

	void Update_State();
	void Update_Sampler();
	void Update_LightShader();
	void Update_TextureShader();
	void Update_Input();
	
private:
	CDevice*				m_pGraphicDevice;
	CInput*					m_pInput;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	ID3D11RasterizerState*	m_pState;
	ID3D11RasterizerState*  m_pStateWireFrame;
	ID3D11SamplerState*		m_pSampler;

	COMHASHMAP				m_mapComponent;

	shared_ptr<CCamera>		m_pCamera;
	shared_ptr<CTimer>		m_pMainTimer;
	shared_ptr<CBox>		m_pBox;
	shared_ptr<CTerrain>	m_pTerrain;
	shared_ptr<CSphere>		m_pSphere;
	shared_ptr<CVisible>	m_pVisible;

private:
	CShader*				m_pLightShader;
	CShader*				m_pTextureShader;

	ID3D11Buffer*			m_pCBLight;
	ID3D11Buffer*			m_pCBPointLight;
	ID3D11Buffer*			m_pCBPerFrame;

	LIGHT					m_Light;
	POINTLIGHT				m_PointLight;
	PERFRAME				m_PerFrame;
	
private:
	UINT					m_dwFrameCnt;

	float					m_fElapsedTime;

	BOOL					m_isWireFrame;
};

