#include "Terrain.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"

CTerrain::CTerrain(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pNormalTexture(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
{
}

CTerrain::~CTerrain()
{
	Release();
}

void CTerrain::Init()
{
	assert(m_pMapComponent);

	// 메쉬
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("TerrainTexMesh")->second->Clone());

	// 트랜스폼
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());

	// 쉐이더
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("DisplacementFX")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 128.f);

	// 텍스쳐
	m_pTexture = static_cast<CTexture*>(m_pMapComponent->find("TerrainTexture")->second->Clone());

	m_pNormalTexture = static_cast<CTexture*>(m_pMapComponent->find("TerrainTexture_N")->second->Clone());
}

void CTerrain::Update(float p_deltaTime)
{
	m_pTransform->Update_Transform();
}

void CTerrain::LastUpdate(float p_deltaTime)
{
}

void CTerrain::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

	XMStoreFloat4x4(&m_mat.matWorld, m_pTransform->Get_World());
	XMStoreFloat4x4(&m_mat.matWorldRT, InverseTranspose(m_pTransform->Get_World()));
	XMStoreFloat4x4(&m_mat.matWVP, g_matView * g_matProj); // 변위 매핑의 경우 VP를 리턴함
	XMStoreFloat4x4(&m_mat.matTex, m_pTransform->Get_Tex());

	m_pShader->Update_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), m_pCB);
	m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);

	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV());
	m_pContext->PSSetShaderResources(1, 1, m_pNormalTexture->Get_TextureRV());
	m_pContext->DSSetShaderResources(1, 1, m_pNormalTexture->Get_TextureRV());

	m_pMesh->Draw_Mesh();
}

void CTerrain::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pNormalTexture);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
