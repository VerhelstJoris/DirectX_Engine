#include "SkySphere.h"



SkySphere::SkySphere(ModelInfo* modelInfo, D3DXVECTOR4 centerColor , D3DXVECTOR4 apexColor)
{
	m_ModelInfo = modelInfo;

	m_CenterColor = centerColor;
	m_ApexColor = apexColor;

	m_positionX = 0;
	m_positionY = 0;
	m_positionZ = 0;
}


SkySphere::~SkySphere()
{
}


bool SkySphere::Initialize(ID3D11Device* device)
{
	bool result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}


	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_WorldMatrix);

	D3DXMATRIX scaleMatrix;

	D3DXMatrixScaling(&scaleMatrix, 150, 150, 150);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &scaleMatrix);

	return true;
}

void SkySphere::Shutdown()
{
	ReleaseBuffers();

}

void SkySphere::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

bool SkySphere::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_ModelInfo->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_ModelInfo->indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_ModelInfo->vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_ModelInfo->modelType[i].x, m_ModelInfo->modelType[i].y, m_ModelInfo->modelType[i].z);
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_ModelInfo->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_ModelInfo->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void SkySphere::ReleaseBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void SkySphere::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

int SkySphere::GetIndexCount()
{
	return m_ModelInfo->indexCount;
}

void SkySphere::SetPosition(float x, float y, float z)
{
	D3DXMATRIX translationMatrixBack, translationMatrixTo;

	D3DXMatrixTranslation(&translationMatrixBack, -m_positionX, -m_positionY, -m_positionZ);

	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	D3DXMatrixTranslation(&translationMatrixTo, x, y, z);

	//D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrixBack);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrixTo);

	return;

}


