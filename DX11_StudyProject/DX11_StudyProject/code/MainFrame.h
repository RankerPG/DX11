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
class CLake;
class CTrees;

class CMainFrame
{
	typedef unordered_map<LPCSTR, shared_ptr<CComponent>> COMHASHMAP;

public:
	enum class RASTERIZER { SOLIDBACK = 0, SOLIDFRONT, WIREBACK, WIREFRONT };
	enum class BLEND { NONALPHA = 0, ALPHA };
	enum class DEPTHSTENCIL { DEPTH = 0, STENCILON, DRAWSTENCIL, SHADOWON };

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
	void Create_Object();

	void Create_RasterizerState();
	void Create_SamplerState();
	void Create_BlendState();
	void Create_DepthStencilState();

	void Update_RasterizerState();
	void Update_SamplerState();
	void Update_BlendState(UINT	p_dwType);
	void Update_DepthStencilState(DEPTHSTENCIL eDS);

	void Update_LightShader();
	void Update_TextureShader();
	void Update_GeometryShader();
	void Update_BillboardShader();

	void Update_Input();

	void Render_Default();
	void Render_Stencil();
	void Render_Shadow();
	void Render_Color();
	
private:
	CDevice*					m_pGraphicDevice;
	CInput*						m_pInput;
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;
	ID3D11RasterizerState*		m_pState[5];
	ID3D11SamplerState*			m_pSampler;
	ID3D11BlendState*			m_pBlend[3];
	ID3D11DepthStencilState*	m_pDepthStencil[4];

	COMHASHMAP					m_mapComponent;

	shared_ptr<CCamera>			m_pCamera;
	shared_ptr<CTimer>			m_pMainTimer;
	shared_ptr<CBox>			m_pBox;
	shared_ptr<CTerrain>		m_pTerrain;
	shared_ptr<CSphere>			m_pSphere;
	shared_ptr<CVisible>		m_pVisible;
	shared_ptr<CLake>			m_pLake;
	shared_ptr<CTrees>			m_pTrees;

private:
	CShader*					m_pLightShader;
	CShader*					m_pTextureShader;
	CShader*					m_pGeometryShader;
	CShader*					m_pBillboardShader;

	ID3D11Buffer*				m_pCBLight;
	ID3D11Buffer*				m_pCBPointLight;
	ID3D11Buffer*				m_pCBPerFrame;
	ID3D11Buffer*				m_pCBMtrl;

	LIGHT						m_Light;
	POINTLIGHT					m_PointLight;
	PERFRAME					m_PerFrame;
	MATERIAL					m_ShadowMtrl;

private:
	UINT						m_dwFrameCnt;

	float						m_fElapsedTime;

	BOOL						m_isWireFrame;
};

