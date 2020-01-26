#include "Trees.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"
#include "GeometryGenerator.h"

CTrees::CTrees(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
	, m_matArrWorld{ XMFLOAT4X4(), }
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_iIdxCnt(0)
	, m_pInstanceVB(nullptr)
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
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("InstanceShader")->second->Clone());
	m_pShader->Create_ConstantBuffer(&XMFLOAT4X4(), sizeof(XMFLOAT4X4), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 128.f);

	m_pTexture = static_cast<CTexture*>(m_pMapComponent->find("TreeTexture")->second->Clone());

	shared_ptr<CGeometryGenerator> generator(CGeometryGenerator::Create_GeometryGenerator(m_pDevice, m_pContext));

	generator->Create_TexQuad(&m_pVB, &m_pIB, &m_iIdxCnt);

	UINT initCnt = 0;

	XMFLOAT4X4 matWorld(2.f, 0.f, 0.f, 0.f,
						0.f, 6.f, 0.f, 0.f,
						0.f, 0.f, 1.f, 0.f,
						0.f, 2.6f, 0.f, 1.f);

	while (1000 != initCnt)
	{
		int iX = (rand() % 128) - 64;
		int iZ = (rand() % 128) - 64;
		
		if (-10 < iX && 10 > iX && -10 < iZ && 10 > iZ)
		{
			continue;
		}

		matWorld._41 = (float)iX;
		matWorld._43 = (float)iZ;

		m_matArrWorld[initCnt] = matWorld;

		++initCnt;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(m_matArrWorld);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_matArrWorld;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	if (FAILED(m_pDevice->CreateBuffer(&bd, &subData, &m_pInstanceVB)))
	{
		MessageBox(g_hWnd, L"Create Instance Buffer Failed!!", 0, 0);
		return;
	}
}

void CTrees::Update(float p_deltaTime)
{

}

void CTrees::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	UINT stride[2] = { sizeof(TEXVERTEX), sizeof(XMFLOAT4X4) };
	UINT offset[2] = { 0, 0 };

	ID3D11Buffer* vbs[2] = { m_pVB, m_pInstanceVB };

	m_pContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, 250, 0, 0, 0);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(1));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, 250, 0, 0, 250);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(2));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, 250, 0, 0, 500);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(3));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, 250, 0, 0, 750);
}

void CTrees::Release()
{
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pInstanceVB);
	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
