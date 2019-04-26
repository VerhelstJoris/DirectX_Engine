
#include "SimpleObject.h"


SimpleObject::SimpleObject()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
	m_Model = 0;
	m_WorldMatrix = XMMatrixIdentity();
}

SimpleObject::SimpleObject(CylinderShape& c) : SimpleObject()
{
	m_WorldMatrix = XMMatrixTranslationFromVector(XMVectorSet(c.GetPosition(0).x, c.GetPosition(0).y, c.GetPosition(0).z, 1.0f));
	m_Model = new ModelData[c.GetNumVertices()];
	for (int i = 0; i < c.GetNumVertices(); ++i)
	{
		m_Model[i].x = c.GetPosition(i).x;
		m_Model[i].y = c.GetPosition(i).y;
		m_Model[i].z = c.GetPosition(i).z;
		m_Model[i].tu = c.GetTexCoord(i).x;
		m_Model[i].tv = c.GetTexCoord(i).y;
		m_Model[i].nx = c.GetNormal(i).x;
		m_Model[i].ny = c.GetNormal(i).y;
		m_Model[i].nz = c.GetNormal(i).z;
	}

	m_VertexCount = c.GetNumVertices();
	m_Indices = c.GetIndices();
	m_IndexCount = m_Indices.size();
}

SimpleObject::SimpleObject(const SimpleObject& other)
{
}

SimpleObject::~SimpleObject()
{
	// Release the model data.
	ReleaseModel();
	// Shutdown the vertex and index buffers.
	ReleaseBuffers();


}

 void SimpleObject::drawCurrent(ID3D11DeviceContext* context) const
{
	 Render(context);
}


void SimpleObject::Render(ID3D11DeviceContext* deviceContext) const
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


int SimpleObject::GetIndexCount()
{
	return m_IndexCount;
}


bool SimpleObject::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	//unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_VertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	//indices = new unsigned long[m_IndexCount];
	/*if(!indices)
	{
		return false;
	}*/

	// Load the vertex array and index array with data.
	for(i = 0; i < m_VertexCount; ++i)
	{
		vertices[i].position = XMFLOAT3(m_Model[i].x, m_Model[i].y, m_Model[i].z);
		vertices[i].texture = XMFLOAT2(m_Model[i].tu, m_Model[i].tv);
		vertices[i].normal = XMFLOAT3(m_Model[i].nx, m_Model[i].ny, m_Model[i].nz);

		m_Indices.push_back(i);
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_VertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_IndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = m_Indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	m_Indices.clear();

	return true;
}


void SimpleObject::ReleaseBuffers()
{
	// Release the index buffer.
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}
}

void SimpleObject::ReleaseModel()
{
	if(m_Model)
	{
		delete [] m_Model;
		m_Model = 0;
	}

	return;
}

void SimpleObject::SetWorldMatrix(XMMATRIX world)
{
	m_WorldMatrix = world;
}

XMMATRIX SimpleObject::GetWorldMatrix()
{
	return m_WorldMatrix;
}

void SimpleObject::SetModelData(ModelData* data)
{
	m_Model = data;
}

void SimpleObject::SetVertexCount(int count)
{
	m_VertexCount = count;
}

void SimpleObject::SetIndexCount(int count)
{
	m_IndexCount = count;
}



