#include "Lake.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"
#include "Frustum.h"

CLake::CLake(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pTransform_2(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pTexture_2(nullptr)
	, m_pTexture_N(nullptr)
	, m_pTexture_N2(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX_TEX2())
	, m_mtrl(MATERIAL())
	, m_vecTexTrans{XMFLOAT2(), }
{
}

CLake::~CLake()
{
	Release();
}

void CLake::Init()
{
	assert(m_pMapComponent);

	// 메쉬 생성
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("TileQuadNrmMesh")->second->Clone());

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Scale(XMVectorSet(4.f, 4.f, 4.f, 0.f));
	m_pTransform->Set_Trans(XMVectorSet(0.f, -0.9f, 0.f, 1.f));

	m_pTransform->Set_TexScale(XMVectorSet(2.f, 2.f, 0.f, 0.f));
	m_pTransform->Update_Transform();
	m_pTransform_2 = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());

	// 충돌구 반지름 계산
	m_pFrustum = static_cast<CFrustum*>(m_pMapComponent->find("Frustum")->second->Clone());

	BoundingSphere bs;
	bs.Transform(bs, m_pTransform->Get_World());

	m_fRadius = bs.Radius;

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("WaveFX")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX_TEX2), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 0.9f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 1024.f);
	m_mtrl.Env = XMFLOAT4(0.5f, 0.f, 0.f, 0.f);

	// 텍스쳐 복사
	m_pTexture = static_cast<CTexture*>(m_pMapComponent->find("WaterTexture")->second->Clone());
	m_pTexture_2 = static_cast<CTexture*>(m_pMapComponent->find("WaterTexture_2")->second->Clone());

	m_pTexture_N = static_cast<CTexture*>(m_pMapComponent->find("WaveTexture_N")->second->Clone());
	m_pTexture_N2 = static_cast<CTexture*>(m_pMapComponent->find("WaveTexture_N2")->second->Clone());
}

void CLake::Update(float p_deltaTime)
{
	//m_vecTexTrans[0].x += p_deltaTime * 0.02f;
	m_vecTexTrans[0].y += p_deltaTime * 0.02f;
					   
	//m_vecTexTrans[1].x -= p_deltaTime * 0.01f;
	m_vecTexTrans[1].y -= p_deltaTime * 0.01f;

	m_pTransform->Set_TexTrans(XMVectorSet(m_vecTexTrans[0].x, m_vecTexTrans[0].y, 0.f, 0.f));
	m_pTransform->Update_Transform();

	m_pTransform_2->Set_TexTrans(XMVectorSet(m_vecTexTrans[1].x, m_vecTexTrans[1].y, 0.f, 0.f));
	m_pTransform_2->Update_Transform();
}

void CLake::LastUpdate(float p_deltaTime)
{
	m_isVisible = m_pFrustum->Compute_CullingObject(m_pTransform->Get_Trans(), m_fRadius);
	g_dwRenderCnt += m_isVisible;
}

void CLake::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());
	
	m_pShader->Update_Shader();

	XMStoreFloat4x4(&m_mat.matWorld, m_pTransform->Get_World());
	XMStoreFloat4x4(&m_mat.matWorldRT, InverseTranspose(m_pTransform->Get_World()));
	XMStoreFloat4x4(&m_mat.matWVP, g_matView * g_matProj); // ViewProj
	XMStoreFloat4x4(&m_mat.matTex, m_pTransform->Get_Tex());

	XMStoreFloat4x4(&m_mat.matTex2, m_pTransform_2->Get_Tex());

	m_pShader->Update_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX_TEX2), m_pCB);
	m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);

	m_pContext->DSSetShaderResources(2, 1, m_pTexture_N->Get_TextureRV());
	m_pContext->DSSetShaderResources(3, 1, m_pTexture_N2->Get_TextureRV());

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV());
	m_pContext->PSSetShaderResources(1, 1, m_pTexture_2->Get_TextureRV());
	m_pContext->PSSetShaderResources(2, 1, m_pTexture_N->Get_TextureRV());
	m_pContext->PSSetShaderResources(3, 1, m_pTexture_N2->Get_TextureRV());

	m_pMesh->Draw_Mesh();
}

void CLake::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pTransform_2);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pTexture_2);
	SAFE_DELETE(m_pTexture_N);
	SAFE_DELETE(m_pTexture_N2);
	SAFE_DELETE(m_pFrustum);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
