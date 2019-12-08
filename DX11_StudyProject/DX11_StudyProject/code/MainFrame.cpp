#include "MainFrame.h"
#include "device.h"
#include "timer.h"
#include "box.h"
#include "terrain.h"
#include "shader.h"
#include "FigureMesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Sphere.h"
#include "Input.h"
#include "Visible.h"
#include "Texture.h"
#include "Lake.h"

XMMATRIX g_matView, g_matProj;

CMainFrame::CMainFrame(CDevice* p_Device)
	: m_pGraphicDevice(p_Device)
	, m_pInput(CInput::Get_Instance())
	, m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_pState{nullptr, nullptr, nullptr, nullptr}
	, m_pSampler(nullptr)
	, m_pBlend{nullptr, nullptr}
	, m_pDepthStencil{nullptr, nullptr}
	, m_pCamera(nullptr)
	, m_pMainTimer(new CTimer())
	, m_pBox(nullptr)
	, m_pTerrain(nullptr)
	, m_pSphere(nullptr)
	, m_pVisible(nullptr)
	, m_pLake(nullptr)
	, m_pLightShader(nullptr)
	, m_pTextureShader(nullptr)
	, m_pCBLight(nullptr)
	, m_pCBPointLight(nullptr)
	, m_pCBPerFrame(nullptr)
	, m_Light(LIGHT())
	, m_PointLight(POINTLIGHT())
	, m_PerFrame(PERFRAME())
	, m_dwFrameCnt(0)
	, m_fElapsedTime(0.f)
	, m_isWireFrame(FALSE)
{
}

CMainFrame::~CMainFrame()
{
	Release();
}

void CMainFrame::Init()
{
	Create_Device();
	Create_Components();

	Create_RasterizerState();
	Create_SamplerState();
	Create_BlendState();
	Create_DepthStencilState();

	Update_RasterizerState();
	Update_SamplerState();
	Update_BlendState(FALSE);

	Create_Object();
}

void CMainFrame::Update()
{
	m_pMainTimer->Tick();
	Calculate_FPS();
	float deltaTime = m_pMainTimer->Get_DeltaTime();

	Update_Input();

	m_pCamera->Update(deltaTime);
	m_pBox->Update(deltaTime);
	m_pTerrain->Update(deltaTime);
	m_pSphere->Update(deltaTime);
	m_pVisible->Update(deltaTime);
	m_pLake->Update(deltaTime);
}

void CMainFrame::Render()
{
	m_pGraphicDevice->Begin_Render();

	Render_Default();

	Render_Stencil();

	Render_Color();

	m_pGraphicDevice->End_Render();
}

void CMainFrame::Release()
{
	m_mapComponent.clear();

	SAFE_RELEASE(m_pCBLight);
	SAFE_RELEASE(m_pCBPointLight);
	SAFE_RELEASE(m_pCBPerFrame);

	SAFE_RELEASE(m_pState[0]);
	SAFE_RELEASE(m_pState[1]);
	SAFE_RELEASE(m_pSampler);
	SAFE_RELEASE(m_pBlend[0]);
	SAFE_RELEASE(m_pBlend[1]);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}

void CMainFrame::Calculate_FPS()
{
	++m_dwFrameCnt;
	m_fElapsedTime += m_pMainTimer->Get_DeltaTime();

	if (m_fElapsedTime >= 1.f)
	{
		float fFrameTime = 1000.f / m_dwFrameCnt;

		wchar_t wText[64] = L"";
		_stprintf_s(wText, L"FPS : %d / Frame Time : %f(ms)", m_dwFrameCnt, fFrameTime);
		SetWindowText(g_hWnd, wText);

		m_dwFrameCnt = 0;
		m_fElapsedTime = 0.f;
	}
}

void CMainFrame::Create_Device()
{
	if (FALSE == m_pGraphicDevice->Init_Device(1280, 720))
	{
		m_pGraphicDevice->Release_Instance();
		return;
	}

	//전역 행렬 초기화
	g_matView = XMMatrixIdentity();
	g_matProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280.f / 720.f, 1.f, 1000.f);

	//
	m_pDevice = m_pGraphicDevice->Get_Device();
	m_pContext = m_pGraphicDevice->Get_Context();
	m_pDevice->AddRef();
	m_pContext->AddRef();
}

void CMainFrame::Create_Components()
{
	// 컴포넌트 생성
	
	// 쉐이더
	CShader* pShader = CShader::Create_Shader(m_pDevice, m_pContext, L"../Shader/Default.fx", "vs_main", "ps_main", 0);
	m_mapComponent.insert(make_pair("DefaultShader", pShader));

	m_pLightShader = pShader = CShader::Create_Shader(m_pDevice, m_pContext, L"../Shader/Light.fx", "vs_main", "ps_main", 1);
	m_mapComponent.insert(make_pair("LightShader", pShader));

	pShader->Create_ConstantBuffer(&m_Light, sizeof(LIGHT), &m_pCBLight);
	pShader->Create_ConstantBuffer(&m_PointLight, sizeof(POINTLIGHT), &m_pCBPointLight);
	pShader->Create_ConstantBuffer(&m_PerFrame, sizeof(PERFRAME), &m_pCBPerFrame);

	XMVECTOR vDir = XMVectorSet(1.f, -1.f, 1.f, 0.f);
	vDir = XMVector3Normalize(-vDir);
	XMStoreFloat3A(&m_Light.direction, vDir);
	m_Light.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_Light.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_Light.specular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	m_PointLight.position = XMFLOAT3A(0.f, 5.f, 0.f);
	m_PointLight.Range = 20.f;
	m_PointLight.diffuse = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	m_PointLight.ambient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	m_PointLight.specular = XMFLOAT4(1.f, 0.f, 0.f, 1.f);

	m_PerFrame.fogColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	m_PerFrame.fogStart = 5.f;
	m_PerFrame.fogRange = 70.f;

	m_pTextureShader = pShader = CShader::Create_Shader(m_pDevice, m_pContext, L"../Shader/Texture.fx", "vs_main", "ps_main", 2);
	m_mapComponent.insert(make_pair("TextureShader", pShader));
	// 상수 버퍼는 라이트 쉐이더와 공유

	// 메쉬
	CMesh* pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("CubeMesh", pMesh));

	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 1);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("SphereMesh", pMesh));

	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 2);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("TerrainMesh", pMesh));

	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 3);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("CubeTexMesh", pMesh));

	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 4);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("SphereTexMesh", pMesh));

	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 5);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("TerrainTexMesh", pMesh));
	
	pMesh = CFigureMesh::Create_FigureMesh(m_pDevice, m_pContext, 6);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("QuadTexMesh", pMesh));

	// 트랜스폼
	CTransform* pTransform = CTransform::Create_Transform(m_pDevice, m_pContext);
	m_mapComponent.insert(make_pair("Transform", pTransform));

	// 텍스쳐
	CTexture* pTexture = CTexture::Create_Texture(m_pDevice, m_pContext, L"./Texture/WoodBox.dds", FALSE);
	m_mapComponent.insert(make_pair("BoxTexture", pTexture));

	pTexture = CTexture::Create_Texture(m_pDevice, m_pContext, L"./Texture/WireFence.dds");
	m_mapComponent.insert(make_pair("WireTexture", pTexture));

	pTexture = CTexture::Create_Texture(m_pDevice, m_pContext, L"./Texture/earth.bmp");
	m_mapComponent.insert(make_pair("EarthTexture", pTexture));

	pTexture = CTexture::Create_Texture(m_pDevice, m_pContext, L"./Texture/Terrain.png");
	m_mapComponent.insert(make_pair("TerrainTexture", pTexture));

	pTexture = CTexture::Create_Texture(m_pDevice, m_pContext, L"./Texture/Water2.dds", FALSE);
	m_mapComponent.insert(make_pair("WaterTexture", pTexture));
}

void CMainFrame::Create_RasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthClipEnable = TRUE;

	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pState[0])))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
		return;
	}

	rd.FrontCounterClockwise = TRUE;

	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pState[1])))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
		return;
	}

	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.FrontCounterClockwise = FALSE;
	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pState[2])))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
		return;
	}

	rd.FrontCounterClockwise = TRUE;

	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pState[3])))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
		return;
	}
}

void CMainFrame::Create_SamplerState()
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MaxAnisotropy = 1;
	sd.MinLOD = 0;
	sd.MaxLOD = 0;
	sd.MipLODBias = 0;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1;
	sd.BorderColor[1] = 1;
	sd.BorderColor[2] = 1;
	sd.BorderColor[3] = 1;

	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	
	if (FAILED(m_pDevice->CreateSamplerState(&sd, &m_pSampler)))
	{
		MessageBox(g_hWnd, L"Create SamplerState Failed!!", 0, 0);
		return;
	}
}

void CMainFrame::Create_BlendState()
{
	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));

	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

	rtbd.BlendEnable = FALSE;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlend = D3D11_BLEND_ONE;
	rtbd.DestBlend = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	bd.RenderTarget[0] = rtbd;

	if (FAILED(m_pDevice->CreateBlendState(&bd, &m_pBlend[0])))
	{
		MessageBox(g_hWnd, L"Create BlendState Failed!!", 0, 0);
		return;
	}

	//
	rtbd.BlendEnable = TRUE;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;


	bd.RenderTarget[0] = rtbd;

	if (FAILED(m_pDevice->CreateBlendState(&bd, &m_pBlend[1])))
	{
		MessageBox(g_hWnd, L"Create BlendState Failed!!", 0, 0);
		return;
	}

	////
	//rtbd.BlendEnable = TRUE;
	//rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	//rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//rtbd.DestBlend = D3D11_BLEND_SRC_COLOR;
	//rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//rtbd.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	//rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;


	//bd.RenderTarget[0] = rtbd;

	//if (FAILED(m_pDevice->CreateBlendState(&bd, &m_pBlend[2])))
	//{
	//	MessageBox(g_hWnd, L"Create BlendState Failed!!", 0, 0);
	//	return;
	//}
}

void CMainFrame::Create_DepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;

	if (FAILED(m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencil[0])))
	{
		MessageBox(g_hWnd, L"Create DepthStencilState Failed!!", 0, 0);
		return;
	}

	//
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = 0xff;
	dsd.StencilWriteMask = 0xff;

	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsd.BackFace = dsd.FrontFace;

	if (FAILED(m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencil[1])))
	{
		MessageBox(g_hWnd, L"Create DepthStencilState Failed!!", 0, 0);
		return;
	}

	//
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = 0xff;
	dsd.StencilWriteMask = 0xff;

	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	dsd.BackFace = dsd.FrontFace;

	if (FAILED(m_pDevice->CreateDepthStencilState(&dsd, &m_pDepthStencil[2])))
	{
		MessageBox(g_hWnd, L"Create DepthStencilState Failed!!", 0, 0);
		return;
	}
}

void CMainFrame::Create_Object()
{
	// 오브젝트 생성
	m_pMainTimer->Reset();

	m_pCamera.reset(new CCamera(m_pDevice, m_pContext, &m_mapComponent));
	m_pCamera->Init();

	m_pBox.reset(new CBox(m_pDevice, m_pContext, &m_mapComponent));
	m_pBox->Init();

	m_pTerrain.reset(new CTerrain(m_pDevice, m_pContext, &m_mapComponent));
	m_pTerrain->Init();

	m_pSphere.reset(new CSphere(m_pDevice, m_pContext, &m_mapComponent));
	m_pSphere->Init();

	m_pVisible.reset(new CVisible(m_pDevice, m_pContext, &m_mapComponent));
	m_pVisible->Init();

	m_pLake.reset(new CLake(m_pDevice, m_pContext, &m_mapComponent));
	m_pLake->Init();
}

void CMainFrame::Update_RasterizerState()
{
	if (TRUE == m_isWireFrame)
	{
		m_pContext->RSSetState(m_pState[2]);
	}
	else
	{
		m_pContext->RSSetState(m_pState[0]);
	}
}

void CMainFrame::Update_SamplerState()
{
	m_pContext->PSSetSamplers(0, 1, &m_pSampler);
}

void CMainFrame::Update_BlendState(UINT	p_dwType)
{
	float factor[] = { 0.f, 0.f, 0.f, 0.f };

	if (0 == p_dwType)
	{
		m_pContext->OMSetBlendState(m_pBlend[0], factor, 0xffffffff);
	}
	else if (1 == p_dwType)
	{
		m_pContext->OMSetBlendState(m_pBlend[1], factor, 0xffffffff);
	}
}

void CMainFrame::Update_DepthStencilState(DEPTHSTENCIL eDS)
{
	if (DEPTHSTENCIL::DEPTH == eDS)
	{
		m_pContext->OMSetDepthStencilState(m_pDepthStencil[0], 1);
	}
	else if (DEPTHSTENCIL::STENCILON == eDS)
	{
		m_pContext->OMSetDepthStencilState(m_pDepthStencil[1], 1);
	}
	else if (DEPTHSTENCIL::DRAWSTENCIL == eDS)
	{
		m_pContext->OMSetDepthStencilState(m_pDepthStencil[2], 1);
	}
}

void CMainFrame::Update_LightShader()
{
	XMStoreFloat3A(&m_PerFrame.viewPos, m_pCamera->Get_ViewPos());
	m_pLightShader->Update_ConstantBuffer(&m_Light, sizeof(LIGHT), m_pCBLight, 1);
	XMStoreFloat3(&m_PointLight.position, m_pVisible->Get_PointLightPos());
	m_pLightShader->Update_ConstantBuffer(&m_PointLight, sizeof(POINTLIGHT), m_pCBPointLight, 4);
	m_pLightShader->Update_ConstantBuffer(&m_PerFrame, sizeof(PERFRAME), m_pCBPerFrame, 3);
}

void CMainFrame::Update_TextureShader()
{
	XMStoreFloat3A(&m_PerFrame.viewPos, m_pCamera->Get_ViewPos());
	m_pTextureShader->Update_ConstantBuffer(&m_Light, sizeof(LIGHT), m_pCBLight, 1);
	XMStoreFloat3(&m_PointLight.position, m_pVisible->Get_PointLightPos());
	m_pTextureShader->Update_ConstantBuffer(&m_PointLight, sizeof(POINTLIGHT), m_pCBPointLight, 4);
	m_pTextureShader->Update_ConstantBuffer(&m_PerFrame, sizeof(PERFRAME), m_pCBPerFrame, 3);
}

void CMainFrame::Update_Input()
{
	if (m_pInput->Get_DIKPressState(DIK_SPACE))
	{
		m_isWireFrame ^= TRUE;

		Update_RasterizerState();
	}
}

void CMainFrame::Render_Default()
{
	Update_TextureShader();
	Update_RasterizerState();
	Update_BlendState(0);
	Update_DepthStencilState(DEPTHSTENCIL::DEPTH);

	// 그리기
	m_pTerrain->Render();
	m_pSphere->Render();

	// 알파 블렌딩
	Update_BlendState(1);
	m_pBox->Render();

	if (TRUE == m_isWireFrame)
		m_pContext->RSSetState(m_pState[3]);
	else
		m_pContext->RSSetState(m_pState[1]);

	m_pBox->Render();

	if (TRUE == m_isWireFrame)
		m_pContext->RSSetState(m_pState[2]);
	else
		m_pContext->RSSetState(m_pState[0]);
}

void CMainFrame::Render_Stencil()
{
	// 스텐실
	Update_DepthStencilState(DEPTHSTENCIL::STENCILON);

	Update_BlendState(1);
	m_pLake->Render();

	// 반사 객체 그리기
	Update_DepthStencilState(DEPTHSTENCIL::DRAWSTENCIL);

	XMVECTOR stencilPlane = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMMATRIX R = XMMatrixReflect(stencilPlane);

	XMVECTOR litDir = XMLoadFloat3A(&m_Light.direction);
	XMVECTOR litReflectDir = XMVector3TransformNormal(litDir, R);
	XMStoreFloat3A(&m_Light.direction, litReflectDir);

	Update_LightShader();
	Update_TextureShader();

	m_pContext->RSSetState(m_pState[1]);
	m_pBox->Render(&R);
	m_pSphere->Render(&R);

	if (TRUE == m_isWireFrame)
		m_pContext->RSSetState(m_pState[2]);
	else
		m_pContext->RSSetState(m_pState[0]);

	m_pBox->Render(&R);

	if (TRUE == m_isWireFrame)
		m_pContext->RSSetState(m_pState[3]);
	else
		m_pContext->RSSetState(m_pState[1]);

	m_pContext->RSSetState(m_pState[0]);
	Update_DepthStencilState(DEPTHSTENCIL::DEPTH);

	Update_BlendState(1);
	m_pLake->Render();

	XMStoreFloat3A(&m_Light.direction, litDir);

	Update_TextureShader();
}

void CMainFrame::Render_Color()
{
	// 컬러 객체
	Update_BlendState(0);
	m_pVisible->Render();
}
