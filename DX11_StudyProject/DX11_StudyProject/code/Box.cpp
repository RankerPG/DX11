#include "Box.h"
#include "Device.h"

CBox::CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context)
	: m_pDevice(p_Device)
	, m_pContext(p_Context)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_inputLayout(nullptr)
{
	m_pDevice->Release();
	m_pContext->Release();

	XMMATRIX mI = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, mI);
	XMStoreFloat4x4(&mView, mI);
	XMStoreFloat4x4(&mProj, mI);
}

CBox::~CBox()
{
	Release();
}

void CBox::Init()
{
	// 버텍스 버퍼 생성
	Vertex vertices[] =
	{
		{ XMFLOAT4(-1.f, -1.f, -1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f) },
		{ XMFLOAT4(-1.f, +1.f, -1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, +1.f, -1.f, 1.f), XMFLOAT4(1.f, 0.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, -1.f, -1.f, 1.f), XMFLOAT4(0.f, 1.f, 0.f, 1.f) },
		{ XMFLOAT4(-1.f, -1.f, +1.f, 1.f), XMFLOAT4(0.f, 0.f, 1.f, 1.f) },
		{ XMFLOAT4(-1.f, +1.f, +1.f, 1.f), XMFLOAT4(1.f, 1.f, 0.f, 1.f) },
		{ XMFLOAT4(+1.f, +1.f, +1.f, 1.f), XMFLOAT4(0.f, 1.f, 1.f, 1.f) },
		{ XMFLOAT4(+1.f, -1.f, +1.f, 1.f), XMFLOAT4(1.f, 0.f, 1.f, 1.f) },
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;

	if (FAILED(m_pDevice->CreateBuffer(&vbd, &vinitData, &m_pVB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}

	// 인덱스 버퍼 생성
	UINT indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	if (FAILED(m_pDevice->CreateBuffer(&ibd, &iinitData, &m_pIB)))
	{
		m_pVB->Release();
		MessageBox(g_hWnd, L"Create Index Buffer Failed!!", 0, 0);
		return;
	}

	// 쉐이더 컴파일
	DWORD shaderFlags = 0;

#ifdef DEBUG | _DEBUG
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledVS = nullptr;
	ID3D10Blob* compilationMsgs = nullptr;

	UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

	HRESULT hr = D3DCompileFromFile(L"./Shader/Default.fx", 0, 0, "vs_main", "vs_5_0", flags, 0, &compiledVS, &compilationMsgs);

	if (nullptr != compilationMsgs)
	{
		m_pVB->Release();
		m_pIB->Release();
		compilationMsgs->Release();
		MessageBoxA(g_hWnd, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		return;
	}

	if (FAILED(hr))
	{
		m_pVB->Release();
		m_pIB->Release();
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

	ID3D10Blob* compiledPS = nullptr;
	hr = D3DCompileFromFile(L"./Shader/Default.fx", 0, 0, "ps_main", "ps_5_0", flags, 0, &compiledPS, &compilationMsgs);

	if (FAILED(m_pDevice->CreatePixelShader(compiledPS->GetBufferPointer(),
		compiledPS->GetBufferSize(),
		nullptr,
		&m_pPS)))
	{
		MessageBox(g_hWnd, L"Create Pixel Shader Failed!!", 0, 0);
		return;
	}
	//상수버퍼 설정
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(XMMATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = &matWVP;
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &sd, &m_pCB)))
	{
		MessageBox(g_hWnd, L"Create Constant Buffer Failed!!", 0, 0);
		return;
	}

	//레이아웃 설정
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(m_pDevice->CreateInputLayout(vertexDesc, 2, compiledVS->GetBufferPointer(),
		compiledVS->GetBufferSize(), &m_inputLayout)))
	{
		MessageBox(g_hWnd, L"Create InputLayout Failed!!", 0, 0);
		return;
	}

	compiledVS->Release();
	compiledPS->Release();
}

void CBox::Update()
{
	XMVECTOR pos = XMVectorSet(0.f, 10.f, -20.f, 1.f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMMATRIX matView = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, matView);

	XMMATRIX matProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280.f / 720.f, 1.f, 1000.f);
	XMStoreFloat4x4(&mProj, matProj);
}

void CBox::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_inputLayout);

	m_pContext->VSSetShader(m_pVS, nullptr, 0);
	m_pContext->PSSetShader(m_pPS, nullptr, 0);
	XMMATRIX matWorld = XMLoadFloat4x4(&mWorld);
	XMMATRIX matView = XMLoadFloat4x4(&mView);
	XMMATRIX matProj = XMLoadFloat4x4(&mProj);
	matWVP = matWorld * matView * matProj;

	ID3D11Resource* resource = nullptr;

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	if (FAILED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr)))
	{
		MessageBox(g_hWnd, L"Update Constant Buffer Failed!!", 0, 0);
		return;
	}

	memcpy(mr.pData, &matWVP, sizeof(XMMATRIX));

	m_pContext->Unmap(m_pCB, 0);

	m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);

	m_pContext->DrawIndexed(36, 0, 0);

	//for (UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	m_Tech->GetPassByIndex(p)->Apply(0, m_pContext);

	//	m_pContext->DrawIndexed(36, 0, 0);
	//}
}

void CBox::Release()
{
	m_pDevice->Release();
	m_pContext->Release();

	m_pVB->Release();
	m_pIB->Release();

	m_inputLayout->Release();
}
