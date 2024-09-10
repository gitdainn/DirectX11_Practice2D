#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CLine : public CGameObject
{
public:
	CLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLine(const CLine& rhs);
	virtual ~CLine() = default;

public:
	virtual HRESULT Initialize_Prototype();
	HRESULT Initialize(const VertexPositionColor& vDestVertex, const VertexPositionColor& vSourVertex, void* pArg);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

#ifdef _DEBUG
protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	VertexPositionColor	m_DestVertex;
	VertexPositionColor	m_SourVertex;

public:
	virtual CGameObject* Clone(const tSpriteInfo& SpriteInfo, void* pArg = nullptr) const;
	virtual CGameObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

END
