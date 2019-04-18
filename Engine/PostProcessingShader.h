
#ifndef _TEXTURESHADERCLASS_H_
#define _TEXTURESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: PostProcessingShader
////////////////////////////////////////////////////////////////////////////////
class PostProcessingShader
{
private:
	struct ConstantBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct ConstantDataBuffer
	{
		float amountOfColumns;
		float vignetteStrength;
		float time;
		float speed;

		float distortionStrength;
		float padding0;
		float padding1;
		float padding2;
	};

public:
	PostProcessingShader();
	PostProcessingShader(const PostProcessingShader&);
	~PostProcessingShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

	void IncrementCount(bool);
	void IncrementSpeed(bool);
	void IncrementBrightness(bool);
	void IncrementDistortion(bool);
	void UpdateTimer(float);
	void Toggle();
	bool GetToggledOn() { return m_ToggledOn; };

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_constantBuffer;
	ID3D11Buffer* m_DataBuffer;

	ID3D11SamplerState* m_sampleState;

	//
	float m_Count = 256;
	float m_Time = 0.0f;
	float m_Speed = 0.1f;
	float m_Brightness = 4.0f;
	float m_DistortionStrength = 1.2f;
	bool m_ToggledOn = true;
};

#endif