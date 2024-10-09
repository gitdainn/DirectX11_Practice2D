#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;


private: /* �� ������ ����ϱ����� �纻 ��ü���� �׷캰�� �����Ѵ�. */
	HRESULT Ready_LightDesc();
	HRESULT Ready_Layer_Camera(const _uint iLayerBitset);
	HRESULT Ready_Layer_BackGround(const _uint iLayerBitset);
	HRESULT Ready_Layer_Player(const _uint iLayerBitset);
	HRESULT Ready_Layer_Monster(const _uint iLayerBitset);
	HRESULT Ready_Layer_Effect(const _uint iLayerBitset);

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END