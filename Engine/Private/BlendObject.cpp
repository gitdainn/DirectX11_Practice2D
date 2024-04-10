#include "..\Public\BlendObject.h"
#include "PipeLine.h"

CBlendObject::CBlendObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlendObject::CBlendObject(const CBlendObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void * pArg)
{
	return S_OK;
}

_uint CBlendObject::Tick(_double TimeDelta)
{
	return _uint();
}

_uint CBlendObject::LateTick(_double TimeDelta)
{
	return _uint();
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}


void CBlendObject::Compute_ViewZ(_fvector vPosition)
{
	CPipeLine*		pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4		vCamPosition = pPipeLine->Get_CamPosition();

	m_fViewZ = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vCamPosition) - vPosition));

	Safe_Release(pPipeLine);

}

CGameObject* CBlendObject::Clone(const tSpriteInfo& SpriteInfo, void* pArg)
{
	return nullptr;
}

void CBlendObject::Free()
{
	__super::Free();

}
