#include "..\Public\VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{

}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance & rhs)
	: CVIBuffer(rhs)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(_uint iNumInstance)
{
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iIndexSizePrimitive = sizeof(FACEINDICES16);
	m_iNumInstances = iNumInstance;
	m_iNumPrimitives = 2 * m_iNumInstances;
	m_iNumIndicesPrimitive = 3;
	m_iNumBuffers = 2;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	/* D3D11_USAGE_DEFAULT : CPU에 의한 값 갱신이 불가능하다 */ 
	/* D3D11_USAGE_DYNAMIC : CPU(C++코드, Lock, unLock)에 의한 값 갱신이 가능하다. */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, m_iStride * m_iNumVertices);

	/* 로컬스페이스 상의 위치 정보를 채운다. */
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.0f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.0f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);

	m_SubResourceData.pSysMem = pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexSizePrimitive * m_iNumPrimitives;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES16*			pIndices = new FACEINDICES16[m_iNumPrimitives];

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumInstances; ++i)
	{
		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 1;
		pIndices[iNumFaces]._2 = 2;
		++iNumFaces;

		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 2;
		pIndices[iNumFaces]._2 = 3;
		++iNumFaces;
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstances;
	/* D3D11_USAGE_DEFAULT : CPU에 의한 값 갱신이 불가능하다 */
	/* D3D11_USAGE_DYNAMIC : CPU(C++코드, Lock, unLock)에 의한 값 갱신이 가능하다. */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[m_iNumInstances];
	ZeroMemory(pInstanceVertices, m_iInstanceStride * m_iNumInstances);

	for (_uint i = 0; i < m_iNumInstances; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vTranslation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	}

	m_SubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
	
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void * pArg)
{
	memcpy(&m_RectInstanceDesc, pArg, sizeof(RECT_INSTANCE_DESC));

	m_pSpeed = new _float[m_iNumInstances];

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstances; ++i)
	{
		m_pSpeed[i] = rand() % int(m_RectInstanceDesc.fMaxSpeed - m_RectInstanceDesc.fMinSpeed) + m_RectInstanceDesc.fMinSpeed;

		_float	fHalfWidth = m_RectInstanceDesc.vSize.x * 0.5f;
		_float	fHalfHeight = m_RectInstanceDesc.vSize.y * 0.5f;		

		// (rand() % (max - min) + min)

			((VTXMATRIX*)SubResource.pData)[i].vTranslation = _float4(rand() % 
			_int(m_RectInstanceDesc.vPosition.x + 
				fHalfWidth - (m_RectInstanceDesc.vPosition.x - fHalfWidth)) + m_RectInstanceDesc.vPosition.x - fHalfWidth,
				m_RectInstanceDesc.vPosition.y, 
				rand() % _int(m_RectInstanceDesc.vPosition.z + fHalfHeight - (m_RectInstanceDesc.vPosition.z - fHalfHeight)) + m_RectInstanceDesc.vPosition.z - fHalfHeight,
				1.f);
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	



	

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Render()
{
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);	

	m_pContext->DrawIndexedInstanced(m_iNumIndicesPrimitive * 2, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Rect_Instance::Update(_double TimeDelta)
{
	

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstances; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * (_float)TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vTranslation.y < 0.0f)
			((VTXMATRIX*)SubResource.pData)[i].vTranslation.y = m_RectInstanceDesc.vPosition.y;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Rect_Instance * CVIBuffer_Rect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Rect_Instance *	pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_Instance::Clone(void * pArg)
{
	CVIBuffer_Rect_Instance *	pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pSpeed);
	Safe_Release(m_pVBInstance);

}
