#include "stdafx.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"

USING(Tool)

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CCamera_Dynamic - Initialize() - No Argument");
		return E_FAIL;
	}

	memcpy(&m_tCameraDynamicDesc, pArg, sizeof m_tCameraDynamicDesc);

	if (FAILED(__super::Initialize(&m_tCameraDynamicDesc.tCameraDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Dynamic::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	const _float fSpeed = { 7.f };
	if (pGameInstance->Get_KeyStay(DIK_W))
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Go_Up(TimeDelta * fSpeed);
		_vector vMovePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(0.f, -1 * abs(XMVectorGetY(vPosition) - XMVectorGetY(vMovePosition)));
		//pGameInstance->Set_Scroll(fScroll);
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());
	}

	if (pGameInstance->Get_KeyStay(DIK_S))
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Go_Down(TimeDelta * fSpeed);
		_vector vMovePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(0.f, abs(XMVectorGetY(vPosition) - XMVectorGetY(vMovePosition)));
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());
		//pGameInstance->Set_Scroll(fScroll);
	}

	if (pGameInstance->Get_KeyStay(DIK_A))
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Go_Left(TimeDelta * fSpeed);
		_vector vMovePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(-1 * abs(XMVectorGetX(vPosition) - XMVectorGetX(vMovePosition)), 0.f);
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());
		//pGameInstance->Set_Scroll(fScroll);
	}

	if (pGameInstance->Get_KeyStay(DIK_D))
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Go_Right(TimeDelta * fSpeed);
		_vector vMovePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float2 fScroll = _float2(abs(XMVectorGetX(vPosition) - XMVectorGetX(vMovePosition)), 0.f);
		//pGameInstance->Scroll_Line(pGameInstance->Get_ScrollX(), pGameInstance->Get_ScrollY());
		//pGameInstance->Set_Scroll(fScroll);
	}

	Safe_Release(pGameInstance);

	__super::Tick(TimeDelta);

	return _uint();
}

_uint CCamera_Dynamic::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	return _uint();
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

CCamera_Dynamic* CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Dynamic::Clone(void* pArg) const
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();


}
