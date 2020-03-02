#include "Rain.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"
#include "Frustum.h"

CRain::CRain(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pRandomTexture(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
{
}

CRain::~CRain()
{
	Release();
}

void CRain::Init()
{
	assert(m_pMapComponent);

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 1.f));
	m_pTransform->Set_Trans(XMVectorSet(0.f, 1.f, 0.f, 1.f));
	m_pTransform->Update_Transform();

	BoundingSphere bs;
	bs.Transform(bs, m_pTransform->Get_World());

	m_fRadius = bs.Radius;

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("NrmMapFX")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 32.f);

	// 텍스쳐 복사
	m_pRandomTexture = static_cast<CTexture*>(m_pMapComponent->find("RandomTexture")->second->Clone());
}

void CRain::Update(float p_deltaTime)
{
	m_pTransform->Acc_Rotation(XMVectorSet(0.f, p_deltaTime * 0.25f, 0.f, 0.f));

	m_pTransform->Update_Transform();
}

void CRain::LastUpdate(float p_deltaTime)
{

}

void CRain::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	//m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	//m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->IASetInputLayout(m_pShader->Get_Layout());

	m_pShader->Update_Shader();

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

	XMStoreFloat4x4(&m_mat.matTex, m_pTransform->Get_Tex());

	m_pShader->Update_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), m_pCB);
	if (TRUE == p_isUseMtrl)
	{
		m_pShader->Update_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), m_pCBMtrl, 2);
	}
	m_pContext->PSSetShaderResources(0, 1, m_pRandomTexture->Get_TextureRV());

	//m_pMesh->Draw_Mesh();
}

void CRain::Release()
{
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pRandomTexture);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
