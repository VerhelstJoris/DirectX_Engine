#include "Line.h"

#include <iostream>

Line::Line(ID3D11Device* device, std::vector<float3> points, float3 color)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_vertexCount = points.size();
	m_indexCount = (points.size() *2) -2;

	m_Points = points;
	m_Color = color;

	Initialize(device);

}

Line::Line(const Line& other)
{
}

Line::~Line()
{
}

bool Line::Initialize(ID3D11Device* device)
{
	bool result;


	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	D3DXMatrixIdentity(&m_worldMatrix);

	return true;
}

void Line::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void Line::Render(ID3D11DeviceContext* deviceContext)
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

int Line::GetIndexCount()
{
	return m_indexCount;
}

bool Line::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}



	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}


	// Load the vertex array and index array with data.
	int indexCount = 1;
	int indexVal = 1;
	indices[0] = 0;
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_Points[i].x, m_Points[i].y, m_Points[i].z);
		vertices[i].color = D3DXVECTOR3(m_Color.x, m_Color.y, m_Color.z);

		/*if (i == 0)
		{
			indices[0] = i;

			indexCount++;
			indexVal++;
		}
		else*/ if (i < m_vertexCount - 1 && i!=0)
		{
			indices[indexCount] = indexVal;
			indices[indexCount + 1] = indexVal;

			indexCount += 2;
			indexVal++;
		}
		else if (i == m_vertexCount - 1 && i!=0)
		{
			indices[indexCount] = indexVal;
		}
	}


	//std::cout << "VERTICES: " << m_vertexCount << std::endl;
	////cout indices
	//std::cout << "INDICES: [";
	//
	//for (size_t i = 0; i < m_indexCount; i++)
	//{
	//	std::cout << indices[i] << ", ";
	//}
	//
	//std::cout << "]" << std::endl;


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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

void Line::ShutdownBuffers()
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

