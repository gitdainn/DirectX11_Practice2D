#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iStride(rhs.m_iStride)
	, m_iNumBuffers(rhs.m_iNumBuffers)
	, m_iNumPrimitives(rhs.m_iNumPrimitives)
	, m_iIndexSizePrimitive(rhs.m_iIndexSizePrimitive)
	, m_iNumIndicesPrimitive(rhs.m_iNumIndicesPrimitive)
	, m_eFormat(rhs.m_eFormat)
	, m_eTopology(rhs.m_eTopology)
{	
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,				
	};

	_uint				iStrides[] = {
		m_iStride, 		
	};

	_uint				iOffsets[] = {
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumIndicesPrimitive * m_iNumPrimitives, 0, 0);
	
	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr != m_pVB)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr != m_pIB)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
