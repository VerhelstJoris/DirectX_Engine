////////////////////////////////////////////////////////////////////////////////
// Filename: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "PostProcessingShader.h"
#include <iostream>

PostProcessingShader::PostProcessingShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_constantBuffer = 0;
	m_sampleState = 0;
}


PostProcessingShader::PostProcessingShader(const PostProcessingShader& other)
{
}


PostProcessingShader::~PostProcessingShader()
{
}


bool PostProcessingShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/crtVS.hlsl", L"../Engine/crtPS.hlsl");
	if(!result)
	{
		return false;
	}

	return true;
}


void PostProcessingShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool PostProcessingShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
								D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool PostProcessingShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "crtVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "crtPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, 
										&m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
									   &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
									   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	//CREATE DATA LAYOUT
	//-------------------------------------------------------------------------------------
	D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof(ConstantDataBuffer);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	result = device->CreateBuffer(&bd, nullptr, &m_DataBuffer);

	if (FAILED(result))
	{
		MessageBox(hwnd, psFilename, L"Can't create extra data buffer", MB_OK);
		return false;
	}	
	
	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

    // Setup the description of the dynamic constant buffer that is in the vertex shader.
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void PostProcessingShader::ShutdownShader()
{
	// Release the sampler state.
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the constant buffer.
	if(m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_DataBuffer)
	{
		m_DataBuffer->Release();
		m_DataBuffer = 0;
	}

	return;
}


void PostProcessingShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool PostProcessingShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
											 D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	ConstantDataBuffer* dataBufferPtr;
	unsigned int bufferNumber;


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_constantBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//EXTRA DATA
	//===============================================
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_DataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataBufferPtr = (ConstantDataBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataBufferPtr->amountOfColumns = m_Count;
	dataBufferPtr->vignetteStrength = m_Brightness;
	dataBufferPtr->time = m_Time;
	dataBufferPtr->speed = m_Speed;
	dataBufferPtr->distortionStrength = m_DistortionStrength;
	dataBufferPtr->padding0 = 0.0f;
	dataBufferPtr->padding1 = 0.0f;
	dataBufferPtr->padding2 = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_DataBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_DataBuffer);

	return true;
}


void PostProcessingShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void PostProcessingShader::IncrementCount(bool upTick)
{
	if (upTick)
	{
		m_Count++;
	}
	else
	{
		m_Count--;
	}
	std::cout << "COUNT: " << m_Count << std::endl;

}

void PostProcessingShader::IncrementSpeed(bool upTick)
{
	if (upTick)
	{
		m_Speed+=0.01f;
	}
	else
	{
		if (m_Speed > 0.02f)
		{
			m_Speed -= 0.01f;
		}
	}
	std::cout << "SPEED: " << m_Speed << std::endl;
}

void PostProcessingShader::IncrementBrightness(bool upTick)
{
	if (upTick)
	{
		m_Brightness += 0.1f;
	}
	else
	{
		if (m_Brightness > 1.0f)
		{
			m_Brightness -= 0.1f;
		}
	}
	std::cout << "Brightness: " << m_Brightness << std::endl;
}

void PostProcessingShader::IncrementDistortion(bool upTick)
{
	if (upTick)
	{
		m_DistortionStrength += 0.01f;
	}
	else
	{
		if (m_DistortionStrength > 0)
		{
			m_DistortionStrength -= 0.01f;
		}
	}
	std::cout << "Distortion: " << m_DistortionStrength << std::endl;
}



void PostProcessingShader::UpdateTimer(float deltaTime)
{
	m_Time+= deltaTime;
}

void PostProcessingShader::Toggle()
{
	m_ToggledOn = !m_ToggledOn;

	std::cout << "Toggled On: " << m_ToggledOn << std::endl;
}
