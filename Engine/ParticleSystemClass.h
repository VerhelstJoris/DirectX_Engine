////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "Structs.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleSystemClass
////////////////////////////////////////////////////////////////////////////////
class ParticleSystemClass
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float velocity;
		float lifetime;
		float size;
		bool active;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	ParticleSystemClass();
	ParticleSystemClass(const ParticleSystemClass&);
	~ParticleSystemClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	bool Frame(const GameContext& context, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

	void SetPosition(float, float, float);

private:
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	void SortParticles(ParticleType Arr[], int l, int r);
	void Swap(ParticleType* arr, int i, int j);
	float DistanceBetweenPoints(D3DXVECTOR3 point1, D3DXVECTOR3 point2);

	bool UpdateBuffers(ID3D11DeviceContext*);

	void RenderBuffers(ID3D11DeviceContext*);

private:

	D3DXVECTOR3 m_StartPosition;
	float m_particleSpawnDeviationX, m_particleSpawnDeviationY, m_particleSpawnDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	float m_MaxSizevariation;

	int m_currentParticleCount;
	float m_accumulatedTime;

	TextureClass* m_Texture;

	ParticleType* m_particleList;

	int m_VertexCount, m_IndexCount;
	VertexType* m_Vertices;
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;

	D3DXVECTOR3 m_CameraPos, m_CameraUp, m_CameraRight, m_CamForward;
};

#endif