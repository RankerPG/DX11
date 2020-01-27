#include "Box.h"
#include "Device.h"
#include "FigureMesh.h"
#include "Shader.h"
#include "transform.h"
#include "Texture.h"
#include "Frustum.h"

CBox::CBox(ID3D11Device* p_Device, ID3D11DeviceContext* p_Context, COMHASHMAP* p_hashMap)
	: CObject(p_Device, p_Context, p_hashMap)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pTexture(nullptr)
	, m_pFrustum(nullptr)
	, m_pCB(nullptr)
	, m_pCBMtrl(nullptr)
	, m_mat(TRANSMATRIX())
	, m_mtrl(MATERIAL())
{
}

CBox::~CBox()
{
	Release();
}

void CBox::Init()
{
	assert(m_pMapComponent);

	// 메쉬 생성
	m_pMesh = static_cast<CMesh*>(m_pMapComponent->find("CubeTexMesh")->second->Clone());

	// 트랜스폼 생성
	m_pTransform = static_cast<CTransform*>(m_pMapComponent->find("Transform")->second->Clone());
	m_pTransform->Set_Scale(XMVectorSet(1.5f, 1.5f, 1.5f, 1.f));
	m_pTransform->Set_Trans(XMVectorSet(0.f, 1.f, 0.f, 1.f));
	m_pTransform->Update_Transform();

	// 충돌구 반지름 계산
	m_pFrustum = static_cast<CFrustum*>(m_pMapComponent->find("Frustum")->second->Clone());

	BoundingSphere bs;
	bs.Transform(bs, m_pTransform->Get_World());

	m_fRadius = bs.Radius;

	// 쉐이더 생성
	m_pShader = static_cast<CShader*>(m_pMapComponent->find("GeometryShader")->second->Clone());
	m_pShader->Create_ConstantBuffer(&m_mat, sizeof(TRANSMATRIX), &m_pCB);
	m_pShader->Create_ConstantBuffer(&m_mtrl, sizeof(MATERIAL), &m_pCBMtrl);

	m_mtrl.diffuse = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_mtrl.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	m_mtrl.specular = XMFLOAT4(1.f, 1.f, 1.f, 32.f);

	// 텍스쳐 복사
	m_pTexture = static_cast<CTexture*>(m_pMapComponent->find("WireTexture")->second->Clone());
}

void CBox::Update(float p_deltaTime)
{
	m_pTransform->Acc_Rotation(XMVectorSet(0.f, p_deltaTime, 0.f, 0.f));

	m_pTransform->Update_Transform();
}

void CBox::LastUpdate(float p_deltaTime)
{
	m_isVisible = m_pFrustum->Compute_CullingObject(m_pTransform->Get_Trans(), m_fRadius);
	g_dwRenderCnt += m_isVisible;
}

void CBox::Render(XMMATRIX* p_matAdd, BOOL p_isUseMtrl)
{
	m_pContext->IASetVertexBuffers(0, 1, m_pMesh->Get_VertexBuffer(), &m_pMesh->Get_Stride(), &m_pMesh->Get_Offset());
	m_pContext->IASetIndexBuffer(m_pMesh->Get_IndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
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
	m_pContext->PSSetShaderResources(0, 1, m_pTexture->Get_TextureRV());

	m_pMesh->Draw_Mesh();
}

void CBox::Release()
{
	SAFE_DELETE(m_pMesh);
	SAFE_DELETE(m_pTransform);
	SAFE_DELETE(m_pShader);
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pFrustum);

	SAFE_RELEASE(m_pCB);
	SAFE_RELEASE(m_pCBMtrl);
}
