#include "Trees.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"

CTrees::CTrees(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
	, m_vTrans{ XMVECTOR(), }
	, m_matRot{XMMATRIX(), }
{
}

CTrees::~CTrees()
{
	Release();
}

void CTrees::Init()
{
	assert(m_pMapComponent);

	// 메쉬 생성
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("QuadTexMesh")->second->Clone());

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Scale(XMVectorSet(4.f, 6.f, 1.f, 1.f));

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("TextureShader")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), &m_pCB);
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

		m_vTrans[initCnt] = XMVectorSet((float)iX, 2.6f, (float)iZ, 1.f);
		m_matRot[initCnt] = XMMatrixIdentity();
		++initCnt;
	}
}

void CTrees::Update(float p_deltaTime)
{
	for (int i = 0; i < 100; ++i)
	{
		XMVECTOR vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR vLook = XMVector3Normalize((g_matViewWorld.r[3] - m_vTrans[i]));
		XMVECTOR vRight = XMVector3Cross(vUp, vLook);
		m_matRot[i].r[0] = vRight;
		m_matRot[i].r[1] = vUp;
		m_matRot[i].r[2] = vLook;
	}
}

void CTrees::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

	m_pTransform->Update_Transform();
	XMStoreFloat4x4(&m_mat.matTex, m_pTransform->Get_Tex());

	if (TRUE == p_isUseMtrl)
	{
		m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);
	}
	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV());

	for (int i = 0; i < 100; ++i)
	{
		m_pTransform->Set_MatRot(m_matRot[i]);
		m_pTransform->Set_Trans(m_vTrans[i]);
		m_pTransform->Update_Transform_OnlyUseMatrix();

		if (nullptr == p_matAdd)
		{
			XMMATRIX matWorld = m_pTransform->Get_World();
			XMStoreFloat4x4(&m_mat.matWorld, matWorld);
			XMStoreFloat4x4(&m_mat.matWVP, matWorld * g_matView * g_matProj);
			XMStoreFloat4x4(&m_mat.matWorldRT, InverseTranspose(matWorld));
		}
		else
		{
			XMMATRIX matWorld = m_pTransform->Get_World() * (*p_matAdd);
			XMStoreFloat4x4(&m_mat.matWorld, matWorld);
			XMStoreFloat4x4(&m_mat.matWVP, matWorld * g_matView * g_matProj);
			XMStoreFloat4x4(&m_mat.matWorldRT, InverseTranspose(matWorld));
		}

		m_pShader->Update_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), m_pCB);

		m_pMesh->Draw_Mesh();
	}
}

void CTrees::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
