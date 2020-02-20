#include "DCMCreator.h"
#include "MainFrame.h"

UINT CDCMCreator::g_dwTexSize = 1024;

CDCMCreator::CDCMCreator(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pArrCubeMapRTV { nullptr, }
	, m_pCubeMapSRV(nullptr)
	, m_pCubeMapDSV(nullptr)
	, m_Viewport(D3D11_VIEWPORT())
	, m_vUps{ XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f)
	, XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(0.f, 0.f, 1.f)
	, XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f) }
{
}

CDCMCreator::CDCMCreator(const CDCMCreator& rhs)
	: CComponent(rhs)
	, m_pArrCubeMapRTV{ nullptr, }
	, m_pCubeMapSRV(rhs.m_pCubeMapSRV)
	, m_pCubeMapDSV(nullptr)
	, m_Viewport(rhs.m_Viewport)
	, m_vUps{ XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f)
	, XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT3(0.f, 0.f, 1.f)
	, XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f) }
{

}


CDCMCreator::~CDCMCreator()
{
	if (false == m_isClone)
	{
		for (int i = 0; i < 6; ++i)
		{
			SAFE_RELEASE(m_pArrCubeMapRTV[i]);
		}

		SAFE_RELEASE(m_pCubeMapDSV);
		SAFE_RELEASE(m_pCubeMapSRV);
	}
}

HRESULT CDCMCreator::Init_DCM()
{
	// 큐브 텍스쳐 생성
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = g_dwTexSize;
	texDesc.Height = g_dwTexSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* pCubeTex = nullptr;
	
	if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &pCubeTex)))
	{
		MessageBox(g_hWnd, L"Texture Create Failed!!", 0, 0);
		return E_FAIL;
	}

	// 큐브 텍스쳐 배열 원소에 대한 렌더 타겟 생성
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		
		if (FAILED(m_pDevice->CreateRenderTargetView(pCubeTex, &rtvDesc, &m_pArrCubeMapRTV[i])))
		{
			MessageBox(g_hWnd, L"Render Target Create Failed!!", 0, 0);
			SAFE_RELEASE(pCubeTex);
			return E_FAIL;
		}
	}

	// 큐브 텍스쳐 리소스 뷰 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	if (FAILED(m_pDevice->CreateShaderResourceView(pCubeTex, &srvDesc, &m_pCubeMapSRV))) 
	{
		MessageBox(g_hWnd, L"Shader Resource View Create Failed!!", 0, 0);
		SAFE_RELEASE(pCubeTex);
		for (int i = 0; i < 6; ++i)
		{
			SAFE_RELEASE(m_pArrCubeMapRTV[i]);
		}
		return E_FAIL;
	}

	SAFE_RELEASE(pCubeTex);

	// 깊이스텐실뷰 생성
	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = g_dwTexSize;
	depthTexDesc.Height = g_dwTexSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthTex = nullptr;

	if (FAILED(m_pDevice->CreateTexture2D(&depthTexDesc, nullptr, &pDepthTex)))
	{
		MessageBox(g_hWnd, L"Depth Texture Create Failed!!", 0, 0);
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthTex, &dsvDesc, &m_pCubeMapDSV)))
	{
		MessageBox(g_hWnd, L"Depth Stencil View Create Failed!!", 0, 0);
		return E_FAIL;
	}

	SAFE_RELEASE(pDepthTex);

	// 뷰포트 생성
	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width = (float)g_dwTexSize;
	m_Viewport.Height = (float)g_dwTexSize;
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;

	return S_OK;
}

void CDCMCreator::Update_RenderTarget(XMFLOAT3 p_vPos, CMainFrame* p_mainFrame)
{
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(p_vPos.x + 1.f, p_vPos.y, p_vPos.z),
		XMFLOAT3(p_vPos.x - 1.f, p_vPos.y, p_vPos.z),
		XMFLOAT3(p_vPos.x, p_vPos.y + 1.f, p_vPos.z),
		XMFLOAT3(p_vPos.x, p_vPos.y - 1.f, p_vPos.z),
		XMFLOAT3(p_vPos.x, p_vPos.y, p_vPos.z + 1.f),
		XMFLOAT3(p_vPos.x, p_vPos.y, p_vPos.z - 1.f)
	};

	XMMATRIX matView[6];
	for (int i = 0; i < 6; ++i)
	{
		matView[i] = XMMatrixLookAtLH(XMLoadFloat3(&p_vPos), XMLoadFloat3(&targets[i]), XMLoadFloat3(&m_vUps[i]));
	}

	XMMATRIX matProj = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.f, 0.1f, 1000.f);

	//
	ID3D11RenderTargetView* pBeforeRTV;
	ID3D11DepthStencilView* pBeforeDSV;
	m_pContext->OMGetRenderTargets(1, &pBeforeRTV, &pBeforeDSV);

	D3D11_VIEWPORT beforeViewport;
	UINT dwNums = 1;
	m_pContext->RSGetViewports(&dwNums, &beforeViewport);
	m_pContext->RSSetViewports(1, &m_Viewport);

	XMFLOAT4 clearColor(0.f, 0.f, 0.f, 1.f);

	XMMATRIX matBeforeView, matBeforeProj;

	matBeforeView = g_matView;
	matBeforeProj = g_matProj;

	g_matProj = matProj;

	for (int i = 0; i < 6; ++i)
	{
		m_pContext->ClearRenderTargetView(m_pArrCubeMapRTV[i], (float*)&clearColor);
		m_pContext->ClearDepthStencilView(m_pCubeMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		m_pContext->OMSetRenderTargets(1, &m_pArrCubeMapRTV[i], m_pCubeMapDSV);

		g_matView = matView[i];

		p_mainFrame->Render_CubeMap();
	}

	m_pContext->GenerateMips(m_pCubeMapSRV);

	m_pContext->RSSetViewports(1, &beforeViewport);
	m_pContext->OMSetRenderTargets(1, &pBeforeRTV, pBeforeDSV);

	g_matView = matBeforeView;
	g_matProj = matBeforeProj;

	SAFE_RELEASE(pBeforeRTV);
	SAFE_RELEASE(pBeforeDSV);
}

CDCMCreator* CDCMCreator::Create_DCM(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
{
	CDCMCreator* pInstance = new CDCMCreator(p_Device, p_Context);

	if (FAILED(pInstance->Init_DCM()))
	{
		MessageBox(g_hWnd, L"DCMCreator Create Failed!!", 0, 0);
		SAFE_DELETE(pInstance);
	}

	return pInstance;
}
