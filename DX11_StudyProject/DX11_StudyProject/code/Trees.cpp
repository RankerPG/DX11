#include "Trees.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"
#include "GeometryGenerator.h"
#include "Frustum.h"

CTrees::CTrees(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pFrustum(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
	, m_matArrWorld{ XMFLOAT4X4(), }
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_iIdxCnt(0)
	, m_dwRenderCnt(0)
	, m_dwTexTreeCnt{ 0, }
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
	m_pTransform->Update_Transform();

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

	// 충돌구 반지름 계산
	m_pFrustum = static_cast<CFrustum*>(m_pMapComponent->find("Frustum")->second->Clone());

	BoundingSphere bs;
	bs.Transform(bs, m_pTransform->Get_World());

	m_fRadius = bs.Radius;

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
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(m_matArrWorld);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

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

void CTrees::LastUpdate(float p_deltaTime)
{
	m_dwRenderCnt = 0;

	memset(m_dwTexTreeCnt, 0, sizeof(UINT) * 4);

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	if (FAILED(m_pContext->Map(m_pInstanceVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr)))
	{
		MessageBox(g_hWnd, L"Update Instance Buffer Failed!!", 0, 0);
		return;
	}

	XMFLOAT4X4* matRender = reinterpret_cast<XMFLOAT4X4*>(mr.pData);

	for (int i = 0; i < 1000; ++i)
	{
		XMVECTOR vPos = XMVectorSet(m_matArrWorld[i]._41, m_matArrWorld[i]._42, m_matArrWorld[i]._43, 1.f);
		bool isVisible = m_pFrustum->Compute_CullingObject(vPos, m_fRadius);
		
		if (TRUE == isVisible)
		{
			g_dwRenderCnt += m_isVisible;

			matRender[m_dwRenderCnt++] = m_matArrWorld[i];
			++m_dwTexTreeCnt[i / 250];
		}
	}

	m_pContext->Unmap(m_pInstanceVB, 0);
}

void CTrees::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	UINT stride[2] = { sizeof(TEXVERTEX), sizeof(XMFLOAT4X4) };
	UINT offset[2] = { 0, 0 };

	ID3D11Buffer* vbs[2] = { m_pVB, m_pInstanceVB };

	m_pContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
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
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, m_dwTexTreeCnt[0], 0, 0, 0);

	UINT dwStartPos = m_dwTexTreeCnt[0];

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(1));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, m_dwTexTreeCnt[1], 0, 0, dwStartPos);

	dwStartPos += m_dwTexTreeCnt[1];

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(2));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, m_dwTexTreeCnt[2], 0, 0, dwStartPos);

	dwStartPos += m_dwTexTreeCnt[2];

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV(3));
	m_pContext->DrawIndexedInstanced(m_iIdxCnt, m_dwTexTreeCnt[3], 0, 0, dwStartPos);
}

void CTrees::Release()
{
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pFrustum);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pInstanceVB);
	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
