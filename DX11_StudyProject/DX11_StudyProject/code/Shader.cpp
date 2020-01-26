#include "Shader.h"

CShader::CShader(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_pGS(nullptr)
	, m_pInputLayout(nullptr)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pVS(rhs.m_pVS)
	, m_pPS(rhs.m_pPS)
	, m_pGS(rhs.m_pGS)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	m_pVS->AddRef();
	m_pPS->AddRef();
	if (nullptr != m_pGS)
	{
		m_pGS->AddRef();
	}
	m_pInputLayout->AddRef();
}

CShader::~CShader()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pInputLayout);
}

void CShader::Create_VertexShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version, int p_LayoutType)
{
	ID3D10Blob* compiledVS = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(p_filename, 0, 0, p_entrypoint, p_version, flags, 0, &compiledVS, &compilationMsgs);

	if (nullptr != compilationMsgs)
	{
		MessageBoxA(g_hWnd, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		compilationMsgs->Release();
		return;
	}

	if (FAILED(hr))
	{
		SAFE_RELEASE(compiledVS);
		MessageBox(g_hWnd, L"Shader Compile Failed!!", 0, 0);
		return;
	}

	if (FAILED(m_pDevice->CreateVertexShader(compiledVS->GetBufferPointer(),
		compiledVS->GetBufferSize(),
		nullptr,
		&m_pVS)))
	{
		MessageBox(g_hWnd, L"Create Vertex Shader Failed!!", 0, 0);
		return;
	}

	Create_InputLayout(compiledVS, p_LayoutType);

	SAFE_RELEASE(compiledVS);
}

void CShader::Create_PixelShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version)
{
	ID3D10Blob* compiledPS = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(p_filename, 0, 0, p_entrypoint, p_version, flags, 0, &compiledPS, &compilationMsgs);

	if (nullptr != compilationMsgs)
	{
		compilationMsgs->Release();
		MessageBoxA(g_hWnd, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		return;
	}

	if (FAILED(hr))
	{
		SAFE_RELEASE(compiledPS);
		MessageBox(g_hWnd, L"Shader Compile Failed!!", 0, 0);
		return;
	}

	if (FAILED(m_pDevice->CreatePixelShader(compiledPS->GetBufferPointer(),
		compiledPS->GetBufferSize(),
		nullptr,
		&m_pPS)))
	{
		MessageBox(g_hWnd, L"Create Pixel Shader Failed!!", 0, 0);
		return;
	}

	SAFE_RELEASE(compiledPS);
}

void CShader::Create_GeometryShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version)
{
	ID3D10Blob* compiledGS = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(p_filename, 0, 0, p_entrypoint, p_version, flags, 0, &compiledGS, &compilationMsgs);

	if (nullptr != compilationMsgs)
	{
		compilationMsgs->Release();
		MessageBoxA(g_hWnd, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		return;
	}

	if (FAILED(hr))
	{
		compiledGS->Release();
		MessageBox(g_hWnd, L"Shader Compile Failed!!", 0, 0);
		return;
	}

	if (FAILED(m_pDevice->CreateGeometryShader(compiledGS->GetBufferPointer(),
		compiledGS->GetBufferSize(),
		nullptr,
		&m_pGS)))
	{
		MessageBox(g_hWnd, L"Create Pixel Shader Failed!!", 0, 0);
		return;
	}

	SAFE_RELEASE(compiledGS);
}

void CShader::Create_ConstantBuffer(LPVOID p_data, UINT p_size, ID3D11Buffer** p_CB)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = p_size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = p_data;
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &sd, p_CB)))
	{
		MessageBox(g_hWnd, L"Create Constant Buffer Failed!!", 0, 0);
		return;
	}
}

void CShader::Update_Shader()
{
	m_pContext->VSSetShader(m_pVS, nullptr, 0);
	m_pContext->PSSetShader(m_pPS, nullptr, 0);
	if (nullptr != m_pGS)
	{
		m_pContext->GSSetShader(m_pGS, nullptr, 0);
	}
	else
	{
		m_pContext->GSSetShader(nullptr, nullptr, 0);
	}
}

void CShader::Update_ConstantBuffer(LPVOID p_data, UINT p_size, ID3D11Buffer* p_CB, UINT p_slot)
{
	ID3D11Resource* resource = nullptr;

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	if (FAILED(m_pContext->Map(p_CB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr)))
	{
		MessageBox(g_hWnd, L"Update Constant Buffer Failed!!", 0, 0);
		return;
	}

	memcpy(mr.pData, p_data, p_size);

	m_pContext->Unmap(p_CB, 0);

	m_pContext->VSSetConstantBuffers(p_slot, 1, &p_CB);
	if (nullptr != m_pGS)
	{
		m_pContext->GSSetConstantBuffers(p_slot, 1, &p_CB);
	}
	m_pContext->PSSetConstantBuffers(p_slot, 1, &p_CB);
}

void CShader::Create_InputLayout(ID3D10Blob* p_CompileVS, int p_LayoutType)
{
	switch (p_LayoutType)
	{
	case 0:
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 1, p_CompileVS->GetBufferPointer(),
			p_CompileVS->GetBufferSize(), &m_pInputLayout)))
		{
			MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
			return;
		}
	}
		break;
	case 1:
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		 0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 2, p_CompileVS->GetBufferPointer(),
			p_CompileVS->GetBufferSize(), &m_pInputLayout)))
		{
			MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
			return;
		}
	}
		break;
	case 2:
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 3, p_CompileVS->GetBufferPointer(),
			p_CompileVS->GetBufferSize(), &m_pInputLayout)))
		{
			MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
			return;
		}
	}
		break;
	case 3:
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 2, p_CompileVS->GetBufferPointer(),
			p_CompileVS->GetBufferSize(), &m_pInputLayout)))
		{
			MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
			return;
		}
	}
		break;
	case 4:
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
			{ "WORLD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 0,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
			{ "WORLD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 16,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
			{ "WORLD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 32,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
			{ "WORLD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, 48,	D3D11_INPUT_PER_INSTANCE_DATA,	1 },
		};

		if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 7, p_CompileVS->GetBufferPointer(),
			p_CompileVS->GetBufferSize(), &m_pInputLayout)))
		{
			MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
			return;
		}
	}
		break;
	}
}

CShader* CShader::Create_Shader(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, LPCWSTR p_filename, LPCSTR p_VSEntrypoint, LPCSTR p_PSEntrypoint, int p_LayoutType)
{
	CShader* pInstance = new CShader(p_Device, p_Context);

	pInstance->Create_VertexShader(p_filename, p_VSEntrypoint, "vs_5_0", p_LayoutType);
	pInstance->Create_PixelShader(p_filename, p_PSEntrypoint, "ps_5_0");

	return pInstance;
}
