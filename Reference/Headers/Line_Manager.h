#pragma once
#include "Base.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLine_Manager : public CBase
{
private:											
	static CLine_Manager* m_pInstance;

public:												
	static CLine_Manager* GetInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		if (nullptr == m_pInstance) 
		{
			m_pInstance = new CLine_Manager(pDevice, pContext);
		}												
		return m_pInstance;								
	}

	// @qurious - 여기에 static을 쓸 수 없는 이유가 모지?
	unsigned long DestroyInstance(void);

private:
	CLine_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLine_Manager(const CLine_Manager& rhs) = delete;
	virtual ~CLine_Manager() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // _DEBUG

public:
	void	Add_Vertex(const VertexPositionColor& vPosition)
	{
		m_VertexList.emplace_back(vPosition); // 내 기억상.... 컨테이너에 넣을 때 복사본이라.... &원본이여도 괜찮을듯 하지만 일단 지켜보기!
	}

#ifdef _DEBUG
protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	list<VertexPositionColor>	m_VertexList;

public:
	virtual void Free() override;
};

END