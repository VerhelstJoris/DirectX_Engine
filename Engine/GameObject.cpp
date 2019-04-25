////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GameObject.h"
#include <fstream>
using namespace std;


GameObject::GameObject(ID3D11Device* device, ModelInfo* modelInfo, TextureClass* difuseTexture, TextureClass* normalTexture, bool scaleDown)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_DiffuseTexture = difuseTexture;
	m_NormalTexture = normalTexture;
	m_ModelInfo = modelInfo;
	m_model = m_ModelInfo->modelType;
	m_vertexCount = m_ModelInfo->vertexCount;
	m_indexCount = m_ModelInfo->indexCount;

	bool result = Initialize(device);

	if (scaleDown)
	{
		SetScale(0.01, 0.01, 0.01);
	}

}


GameObject::GameObject(const GameObject& other)
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


GameObject::~GameObject()
{
}

bool GameObject::Initialize(ID3D11Device* device)
{
	bool result;


	// Initialize the vertex and index buffers.
	CalculateModelVectors();

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Initialize the world matrix to the identity matrix.
	D3DXMatrixIdentity(&m_WorldMatrix);

	return true;
}

void GameObject::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void GameObject::Render(ID3D11DeviceContext* deviceContext)
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	auto pos = position + lookAt;

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int GameObject::GetIndexCount()
{
	return m_indexCount;
}

bool GameObject::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		indices[i] = i;
	}


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
	if(FAILED(result))
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
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void GameObject::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void GameObject::RenderBuffers(ID3D11DeviceContext* deviceContext)
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


ID3D11ShaderResourceView* GameObject::GetDiffuseTexture()
{
	return m_DiffuseTexture->GetTexture();
}

ID3D11ShaderResourceView* GameObject::GetNormalTexture()
{
	return m_NormalTexture->GetTexture();
}

void GameObject::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_WorldMatrix;
	return;
}

//set position to this 
void GameObject::SetPosition(float x, float y, float z)
{


	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;


	D3DXMATRIX translationMatrix;
	D3DXMatrixTranslation(&translationMatrix, x, y, z);
	//D3DXMatrixTranslation(&translationMatrix2, );


	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix);

	return;

}

//add on top of existing translation
void GameObject::Translate(float x, float y, float z)
{
	m_positionX += x;
	m_positionY += y;
	m_positionZ += z;


	D3DXMATRIX translationMatrix;
	D3DXMatrixTranslation(&translationMatrix, x, y, z);

	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix);

	return;
}

//set the rotation to this
void GameObject::SetRotation(float x, float y, float z)
{
	//translate to 0,0,0
	auto pos = GetPosition();

	D3DXMATRIX translationMatrix1, translationMatrix2;
	D3DXMatrixTranslation(&translationMatrix1, -pos.x, -pos.y, -pos.z);
	D3DXMatrixTranslation(&translationMatrix2, pos.x, pos.y, pos.z);

	//rotation
	D3DXMATRIX rotMatrixX1, rotMatrixY1, rotMatrixZ1;
	x = Deg2Rad(x);
	y = Deg2Rad(y);
	z = Deg2Rad(z);

	D3DXMatrixRotationX(&rotMatrixX1, x);
	D3DXMatrixRotationY(&rotMatrixY1, y);
	D3DXMatrixRotationZ(&rotMatrixZ1, z);

	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;


	//all transforms
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix1);

	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &rotMatrixX1);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &rotMatrixY1);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &rotMatrixZ1);

	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix2);


	return;
}

//adds on top of existing rotation
void GameObject::Rotate(float x, float y, float z)
{
	//translate to 0,0,0
	auto pos = GetPosition();

	D3DXMATRIX translationMatrix1, translationMatrix2;
	D3DXMatrixTranslation(&translationMatrix1, -pos.x, -pos.y, -pos.z);
	D3DXMatrixTranslation(&translationMatrix2, pos.x, pos.y, pos.z);

	x = Deg2Rad(x);
	y = Deg2Rad(y);
	z = Deg2Rad(z);

	//rotate
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;

	D3DXMATRIX rotMatrix;

	D3DXMatrixRotationY(&rotMatrix, y);

	//all transforms
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix1);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &rotMatrix);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &translationMatrix2);

	return;
}

void GameObject::SetScale(float x, float y, float z)
{

	D3DXMATRIX scaleMatrix;

	D3DXMatrixScaling(&scaleMatrix, x, y, z);
	D3DXMatrixMultiply(&m_WorldMatrix, &m_WorldMatrix, &scaleMatrix);

	return;

}

D3DXVECTOR3 GameObject::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 GameObject::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void GameObject::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}

	return;
}

void GameObject::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void GameObject::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}

