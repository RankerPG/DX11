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

XMMATRIX g_matView, g_matProj;

CMainFrame::CMainFrame(CDevice* p_Device)
	: m_pGraphicDevice(p_Device)
	, m_pInput(CInput::Get_Instance())
	, m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_pState(nullptr)
	, m_pStateWireFrame(nullptr)
	, m_pCamera(nullptr)
	, m_pMainTimer(new CTimer())
	, m_pBox(nullptr)
	, m_pTerrain(nullptr)
	, m_pSphere(nullptr)
	, m_pLightShader(nullptr)
	, m_pCBLight(nullptr)
	, m_pCBPerFrame(nullptr)
	, m_Light(LIGHT())
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
	Update_State();
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
}

void CMainFrame::Render()
{
	m_pGraphicDevice->Begin_Render();

	Update_LightShader();

	// 그리기
	m_pTerrain->Render();
	m_pSphere->Render();
	m_pBox->Render();

	m_pGraphicDevice->End_Render();
}

void CMainFrame::Release()
{
	m_mapComponent.clear();

	SAFE_RELEASE(m_pCBLight);
	SAFE_RELEASE(m_pCBPerFrame);

	SAFE_RELEASE(m_pState);
	SAFE_RELEASE(m_pStateWireFrame);
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
	CShader* pShader = new CShader(m_pDevice, m_pContext);
	pShader->Create_VertexShader(L"./Shader/Default.fx", "vs_main", "vs_5_0");
	pShader->Create_PixelShader(L"./Shader/Default.fx", "ps_main", "ps_5_0");
	m_mapComponent.insert(make_pair("DefaultShader", pShader));

	m_pLightShader = pShader = new CShader(m_pDevice, m_pContext);
	pShader->Create_VertexShader(L"./Shader/Light.fx", "vs_main", "vs_5_0", 1);
	pShader->Create_PixelShader(L"./Shader/Light.fx", "ps_main", "ps_5_0");
	m_mapComponent.insert(make_pair("LightShader", pShader));
	pShader->Create_ConstantBuffer(&m_Light, sizeof(LIGHT), &m_pCBLight);
	pShader->Create_ConstantBuffer(&m_PerFrame, sizeof(PERFRAME), &m_pCBPerFrame);

	XMVECTOR vDir = XMVectorSet(1.f, -1.f, 1.f, 0.f);
	vDir = XMVector3Normalize(-vDir);
	XMStoreFloat3A(&m_Light.direction, vDir);
	m_Light.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_Light.ambient = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_Light.specular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

	// 메쉬
	CMesh* pMesh = new CFigureMesh(m_pDevice, m_pContext);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("CubeMesh", pMesh));

	pMesh = new CFigureMesh(m_pDevice, m_pContext, 1);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("SphereMesh", pMesh));

	pMesh = new CFigureMesh(m_pDevice, m_pContext, 2);
	pMesh->Init_Mesh();
	m_mapComponent.insert(make_pair("TerrainMesh", pMesh));

	// 트랜스폼
	CTransform* pTransform = new CTransform(m_pDevice, m_pContext);
	m_mapComponent.insert(make_pair("Transform", pTransform));
}

void CMainFrame::Create_RasterizerState()
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthClipEnable = TRUE;

	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pState)))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
		return;
	}

	rd.FillMode = D3D11_FILL_WIREFRAME;

	if (FAILED(m_pDevice->CreateRasterizerState(&rd, &m_pStateWireFrame)))
	{
		MessageBox(g_hWnd, L"Create RasterizerState Failed!!", 0, 0);
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
}

void CMainFrame::Update_State()
{
	if (TRUE == m_isWireFrame)
	{
		m_pContext->RSSetState(m_pStateWireFrame);
	}
	else
	{
		m_pContext->RSSetState(m_pState);
	}
}

void CMainFrame::Update_LightShader()
{
	XMStoreFloat3A(&m_PerFrame.viewPos, m_pCamera->Get_ViewPos());
	m_pLightShader->Update_ConstantBuffer(&m_Light, sizeof(LIGHT), m_pCBLight, 1);
	m_pLightShader->Update_ConstantBuffer(&m_PerFrame, sizeof(PERFRAME), m_pCBPerFrame, 3);
}

void CMainFrame::Update_Input()
{
	if (m_pInput->Get_DIKPressState(DIK_SPACE))
	{
		m_isWireFrame ^= TRUE;

		Update_State();
	}
}
