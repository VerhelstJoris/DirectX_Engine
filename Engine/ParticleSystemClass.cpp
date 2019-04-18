////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "particlesystemclass.h"
#include "cameraclass.h"

ParticleSystemClass::ParticleSystemClass()
{
	m_Texture = 0;
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}


ParticleSystemClass::~ParticleSystemClass()
{
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;


	// Load the texture that is used for the particles.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if (!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();

	return;
}

bool ParticleSystemClass::Frame(const GameContext& context, ID3D11DeviceContext* deviceContext)
{
	bool result;

	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles(context.deltaTime);

	// Update the position of the particles.
	UpdateParticles(context.deltaTime);

	//sort the particles on distance
	m_CameraPos = context.camera->GetPosition();
	m_CameraRight = context.camera->GetRightVector();
	m_CameraUp = context.camera->GetUpVector();
	m_CamForward = context.camera->GetForwardVector();

	if (m_currentParticleCount > 0)
	{
		SortParticles(m_particleList, 0, m_currentParticleCount - 1);
	}

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return m_Texture->GetTexture();
}

int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystemClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool ParticleSystemClass::InitializeParticleSystem()
{
	int i;

	// Set the random deviation of where the particles can be located when emitted.
	m_particleSpawnDeviationX = 0.15f;
	m_particleSpawnDeviationY = 0.1f;
	m_particleSpawnDeviationZ = 0.15f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.3f;
	m_MaxSizevariation = 0.15f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 25;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 250;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	// Release the particle list.
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = 0;
	}

	return;
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
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

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
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

void ParticleSystemClass::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity;
	int index, i, j;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if (emitParticle && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX =  m_StartPosition.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleSpawnDeviationX;
		positionY =  m_StartPosition.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleSpawnDeviationY;
		positionZ =  m_StartPosition.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleSpawnDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			m_particleList[i].lifetime = m_particleList[j].lifetime;
			m_particleList[i].size = m_particleList[j].size;

			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;

		m_particleList[index].lifetime = 0.0f;

		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;

		m_particleList[index].size = 0.3f;
	}

	return;
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{

	for (size_t i = 0; i < m_currentParticleCount; i++)
	{
		//move the particles up
		m_particleList[i].positionY = m_particleList[i].positionY + (m_particleList[i].velocity * frameTime);
		m_particleList[i].lifetime += frameTime;
		m_particleList[i].positionX = m_particleList[i].positionX + (pow(m_particleList[i].lifetime, 2)* -0.0005f);
		m_particleList[i].positionZ = m_particleList[i].positionZ + (pow(m_particleList[i].lifetime,2)* -0.0005f);
		m_particleList[i].size = (-0.2 * pow((m_particleList[i].lifetime - 5),2) + 5.5 )/20;

	}

	return;
}

void ParticleSystemClass::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < m_maxParticles; i++)
	{
		if (m_particleList[i].lifetime >= 10.0f)
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < m_maxParticles - 1; j++)
			{
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
				m_particleList[j].lifetime = m_particleList[j + 1].lifetime;
				m_particleList[j].size = m_particleList[j + 1].size;

			}
		}
	}

	return;
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i < m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (-m_particleList[i].size)) + (m_CameraUp * (-m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (-m_particleList[i].size)) + (m_CameraUp * (m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (m_particleList[i].size)) + (m_CameraUp * (-m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (m_particleList[i].size)) + (m_CameraUp * (-m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (-m_particleList[i].size)) + (m_CameraUp * (m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		index++;

		// Top right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX, m_particleList[i].positionY, m_particleList[i].positionZ) + (m_CameraRight * (m_particleList[i].size)) + (m_CameraUp * (m_particleList[i].size));
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ParticleSystemClass::SortParticles(ParticleType arr[], int firstIndex, int lastIndex)
{

	//only 1 element in the given arr
	if (firstIndex >= lastIndex)
	{
		return;
	}

	//pivot is element int the middle
	int middleIndex = firstIndex + (lastIndex - firstIndex) / 2;
	//int middleIndex = (firstIndex + lastIndex) / 2;
	float pivotDistance = DistanceBetweenPoints(m_CameraPos, D3DXVECTOR3(arr[middleIndex].positionX, arr[middleIndex].positionY , arr[middleIndex].positionZ));

	int i = firstIndex;
	int j = lastIndex;

	while (firstIndex < j || i < lastIndex)
	{
		while (DistanceBetweenPoints(m_CameraPos, D3DXVECTOR3(arr[i].positionX, arr[i].positionY, arr[i].positionZ)) > pivotDistance)
			i++;
		while (DistanceBetweenPoints(m_CameraPos, D3DXVECTOR3(arr[j].positionX, arr[j].positionY, arr[j].positionZ)) < pivotDistance)
			j--;

		if (i <= j) {
			Swap(arr, i, j);
			i++;
			j--;
		}
		else 
		{
			if (firstIndex < j)
				SortParticles(arr, firstIndex, j);
			if (i < lastIndex)
				SortParticles(arr, i, lastIndex);
			return;
		}
	}
}

void ParticleSystemClass::Swap(ParticleType* arr, int i, int j)
{
	ParticleType temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

float ParticleSystemClass::DistanceBetweenPoints(D3DXVECTOR3 point1, D3DXVECTOR3 point2)
{
	//square root is unnecessary
	//return pow(pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2) + pow((point2.z - point1.z), 2),0.5);
	return pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2) + pow((point2.z - point1.z), 2);
	
	//euclidean distance is not ideal anymore after billboarding
	//maybe distance along camera forward axis??

	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane, &point2, &m_CamForward);

	float dist =  fabs(plane.a * point1.x + plane.b * point1.y + plane.c * point1.z + plane.d);
	dist /= (pow(pow(plane.a, 2) + pow(plane.b, 2) + pow(plane.c, 2), 0.5));
	return dist;

}

void ParticleSystemClass::SetPosition(float x, float y, float z)
{
	m_StartPosition = D3DXVECTOR3(x, y, z);
}
