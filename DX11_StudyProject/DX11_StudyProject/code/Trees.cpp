#include "Trees.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"

CTrees::CTrees(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
	, m_vtx{ GEOVERTEX(), }
{
}

CTrees::~CTrees()
{
	Release();
}

void CTrees::Init()
{
	assert(m_pMapComponent);

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Scale(XMVectorSet(4.f, 6.f, 1.f, 1.f));

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("BillboardShader")->second->Clone());
	m_pShader->Create_ConstantBuffer(&XMFLOAT4X4(), sizeof(XMFLOAT4X4), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 128.f);

	m_pTexture = static_cast<CTexture*>(m_pMapComponent->find("TreeTexture")->second->Clone());

	UINT initCnt = 0;

	while (100 != initCnt)
	{
		int iX = (rand() % 128) - 64;
		int iZ = (rand() % 128) - 64;
		
		if (-10 < iX && 10 > iX && -10 < iZ && 10 > iZ)
		{
			continue;
		}
		m_vtx[initCnt].pos.x = (float)iX;
		m_vtx[initCnt].pos.y = 2.6f;
		m_vtx[initCnt].pos.z = (float)iZ;
		m_vtx[initCnt].pos.w = 1.f;

		m_vtx[initCnt].size.x = 4.f;
		m_vtx[initCnt].size.y = 6.f;

		++initCnt;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(GEOVERTEX) * 100;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_vtx;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, &m_pVB)))
	{
		MessageBox(g_hWnd, L"Create Vertex Buffer Failed!!", 0, 0);
		return;
	}
}

void CTrees::Update(float p_deltaTime)
{
}

void CTrees::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	UINT stride = sizeof(GEOVERTEX);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

	m_pTransform->Update_Transform();
	XMStoreFloat4x4(&m_mat.matTex, m_pTransform->Get_Tex());

	if (TRUE == p_isUseMtrl)
	{
		m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);
	}

	XMMATRIX matViewProj = XMMatrixIdentity();

	if (nullptr != p_matAdd)
	{
		matViewProj = *p_matAdd;
	}

	matViewProj *= g_matView * g_matProj;
	XMStoreFloat4x4(&m_matViewProj, matViewProj);

	m_pShader->Update_ConstantBuffer(&m_matViewProj, sizeof(XMFLOAT4X4), m_pCB);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV());
	m_pContext->Draw(25, 0);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(1));
	m_pContext->Draw(25, 25);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(2));
	m_pContext->Draw(25, 50);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(3));
	m_pContext->Draw(25, 75);
}

void CTrees::Release()
{
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
