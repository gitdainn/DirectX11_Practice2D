#include "stdafx.h"
#include "SkulItem.h"
#include "Line_Manager.h"
#include "FileLoader.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CSkulItem::CSkulItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CSkulItem::CSkulItem(const CSkulItem& rhs)
	: CItem(rhs)
{
}

HRESULT CSkulItem::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	m_pLayerTag = LAYER_ITEM;
	m_eRenderGroup = CRenderer::RENDER_UI;

	return S_OK;
}

HRESULT CSkulItem::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Landing_Ground();
	Add_Animation();

	return S_OK;
}

HRESULT CSkulItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Landing_Ground();
	Add_Animation();

	return S_OK;
}

_uint CSkulItem::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CSkulItem::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CSkulItem::Render()
{
	return __super::Render();
}

void CSkulItem::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	__super::OnCollisionEnter(pTargetCollider, pTarget, pTargetLayer);
}

void CSkulItem::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	if (pGameInstance->Get_KeyDown(DIK_F))
	{
		SPRITE_INFO tSpriteInfo;
		tSpriteInfo.fSize = _float2{ 200.f, 150.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterSkul"), LEVEL_LOGO, LAYER_PLAYER, tSpriteInfo)))
			return;
		list<CGameObject*>* pObjList = pGameInstance->Get_ObjectList(LEVEL_LOGO, LAYER_PLAYER);
		if (nullptr == pObjList)
			return;
		CGameObject* pObject = pObjList->back();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObject);
		if (nullptr == pPlayer)
			return;
		CPlayer_Manager::GetInstance()->Acquire_Skul(pPlayer);
		m_bIsDead = true;
	}

	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
}

void CSkulItem::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
}

void CSkulItem::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

void CSkulItem::Add_Animation()
{
}

void CSkulItem::Landing_Ground()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fLandingY = { 0.f };
	if (pGameInstance->IsCurrentLineOccupied(_float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition)), fLandingY))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
	}

	Safe_Release(pGameInstance);

	return;
}

HRESULT CSkulItem::Add_Components(void* pArg)
{
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_UI_Skul"),
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;

	m_pSpriteFileName = TEXT("AquaSkull_2_Item");

	return __super::Add_Components(pArg);
}

HRESULT CSkulItem::SetUp_ShaderResources()
{
	if (FAILED(SetUp_ShaderDefault()))
		return E_FAIL;

	return S_OK;
}

CSkulItem* CSkulItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkulItem* pInstance = new CSkulItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSkulItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkulItem::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CSkulItem* pInstance = new CSkulItem(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CSkulItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkulItem::Clone(void* pArg) const
{
	CSkulItem* pInstance = new CSkulItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkulItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkulItem::Free()
{
	__super::Free();
}
