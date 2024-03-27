#include "..\Public\VIBuffer_Terrain.h"
#include "Frustum.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pFrustum(CFrustum::GetInstance())	
	
{
	
	Safe_AddRef(m_pFrustum);
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_pFrustum(CFrustum::GetInstance())
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
	Safe_AddRef(m_pFrustum);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER		fh;
	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);

	BITMAPINFOHEADER		ih;
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;	

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iIndexSizePrimitive = sizeof(FACEINDICES32);
	m_iNumPrimitives = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndicesPrimitive = 3;
	m_iNumBuffers = 1;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, m_iStride * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);
	
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.0f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}	

	m_SubResourceData.pSysMem = pVertices;

	Safe_Delete_Array(pPixel);
#pragma endregion

#pragma region INDEX_BUFFER
	FACEINDICES32*			pIndices = new FACEINDICES32[m_iNumPrimitives];
	m_pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	_ulong		dwNumFaces = 0;

	_vector		vSourDir, vDestDir, vNormal;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX, 
				iIndex + m_iNumVerticesX +1,
				iIndex + 1, 
				iIndex
			};

			pIndices[dwNumFaces]._0 = iIndices[0];
			pIndices[dwNumFaces]._1 = iIndices[1];
			pIndices[dwNumFaces]._2 = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vPosition) - 
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._0].vPosition);

			vDestDir = XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._0].vNormal, 
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._2].vNormal) + vNormal);
			
			++dwNumFaces;

			pIndices[dwNumFaces]._0 = iIndices[0];
			pIndices[dwNumFaces]._1 = iIndices[2];
			pIndices[dwNumFaces]._2 = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._0].vPosition);

			vDestDir = XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._2].vPosition) -
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._0].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._1].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[dwNumFaces]._2].vNormal,
				XMLoadFloat3(&pVertices[pIndices[dwNumFaces]._2].vNormal) + vNormal);

			++dwNumFaces;			
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexSizePrimitive * m_iNumPrimitives;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);

	if (FAILED(m_pQuadTree->SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain::Get_Height(_fvector vPosition)
{
	/* vPosition은 지형의 어떤 네모안에 있냐? */
	/* 어떤 네모 == 네모기준 왼쪽 하단 정점의 인덱스 */
	_uint		iIndex = _uint(XMVectorGetZ(vPosition)) * m_iNumVerticesX
		+ _uint(XMVectorGetX(vPosition));

	/* 왼쪽 하단 인덱스 값을 기준으로하여 네 꼭짓점에 해당하는 정점의 인덱스를 모두 구해둔다. */
	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX, 
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1, 
		iIndex
	};

	_float		fWidth, fHeight;

	fWidth = XMVectorGetX(vPosition) - m_pVerticesPos[iIndices[0]].x;
	fHeight = m_pVerticesPos[iIndices[0]].z - XMVectorGetZ(vPosition);

	//ax + by + cz + d = 0

	//y = (-ax - cz - d) / b;	

	_vector		vPlane;
	
	/* 오른쪽 위 삼각형 안에 있냐? */
	if (fWidth > fHeight)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}

	/* 왼쪽 아래 삼각형 안에 있냐? */
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
	}

	return (-(XMVectorGetX(vPlane) * XMVectorGetX(vPosition)) -
		(XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition)) -
		XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	if (nullptr == m_pFrustum)
		return;

	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint		iNumFaces = 0;

	m_pQuadTree->Culling(m_pFrustum, m_pVerticesPos, m_pIndices, &iNumFaces);

	/*for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool		isIn[4] = {
				m_pFrustum->isIn_Local(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 0.f),
				m_pFrustum->isIn_Local(XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), 0.f),
				m_pFrustum->isIn_Local(XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), 0.f),
				m_pFrustum->isIn_Local(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]), 0.f),
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])				
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[1];
				m_pIndices[iNumFaces]._2 = iIndices[2];
				++iNumFaces;				
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				m_pIndices[iNumFaces]._0 = iIndices[0];
				m_pIndices[iNumFaces]._1 = iIndices[2];
				m_pIndices[iNumFaces]._2 = iIndices[3];
				++iNumFaces;
			}
		}
	}*/

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof  SubResource);

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFaces);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumPrimitives = iNumFaces;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain *	pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain *	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);
	Safe_Release(m_pFrustum);

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pIndices);
	}
}
