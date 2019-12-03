#include "Shader.h"

CShader::CShader(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: CComponent(p_Device, p_Context)
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_pCB(nullptr)
	, m_pInputLayout(nullptr)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs.m_pDevice, rhs.m_pContext)
	, m_pVS(rhs.m_pVS)
	, m_pPS(rhs.m_pPS)
	, m_pCB(rhs.m_pCB)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	m_pVS->AddRef();
	m_pPS->AddRef();
	m_pCB->AddRef();
	m_pInputLayout->AddRef();
}

CShader::~CShader()
{
	m_pVS->Release();
	m_pPS->Release();
	m_pCB->Release();
	m_pInputLayout->Release();
}

void CShader::Create_VertexShader(LPCWSTR p_filename, LPCSTR p_entrypoint, LPCSTR p_version)
{
	ID3D10Blob* compiledVS = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(p_filename, 0, 0, p_entrypoint, p_version, flags, 0, &compiledVS, &compilationMsgs);

	if (nullptr != compilationMsgs)
	{
		compilationMsgs->Release();
		MessageBoxA(g_hWnd, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		return;
	}

	if (FAILED(hr))
	{
		compiledVS->Release();
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

	Create_InputLayout(compiledVS);

	compiledVS->Release();
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
		compiledPS->Release();
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

	compiledPS->Release();
}

void CShader::Create_ConstantBuffer(LPVOID p_data, UINT p_size)
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

	if (FAILED(m_pDevice->CreateBuffer(&bd, &sd, &m_pCB)))
	{
		MessageBox(g_hWnd, L"Create Constant Buffer Failed!!", 0, 0);
		return;
	}
}

void CShader::Update_Shader()
{
	m_pContext->VSSetShader(m_pVS, nullptr, 0);
	m_pContext->PSSetShader(m_pPS, nullptr, 0);
}

void CShader::Update_ConstantBuffer(LPVOID p_data, UINT p_size)
{
	ID3D11Resource* resource = nullptr;

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	if (FAILED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr)))
	{
		MessageBox(g_hWnd, L"Update Constant Buffer Failed!!", 0, 0);
		return;
	}

	memcpy(mr.pData, p_data, p_size);

	m_pContext->Unmap(m_pCB, 0);

	m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);
}

void CShader::Create_InputLayout(ID3D10Blob* p_CompileVS, int p_LayoutType)
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 2, p_CompileVS->GetBufferPointer(),
		p_CompileVS->GetBufferSize(), &m_pInputLayout)))
	{
		MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
		return;
	}
}