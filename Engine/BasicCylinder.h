#pragma once

#include <directxmath.h>
#include <vector>

using namespace DirectX;

//TO-DO 
//MAKE INHERIT FROM THE GAMEOBJECT CLASS
//=========================================
//TO-DO
//MAKE SIMILAR CLASSES FOR OTHER BASIC SHAPES

class BasicCylinder
{
private:
	struct Vertex
	{
		Vertex() {};
		Vertex(XMFLOAT3 position) : position(position) {};
		Vertex(XMFLOAT3 position, XMFLOAT3 n, XMFLOAT2 texCoord) : position(position), normal(n), texCoord(texCoord) {};

		XMFLOAT3 position;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
	};

public:
	BasicCylinder();
	BasicCylinder(int, int, int);
	~BasicCylinder();

	void GenCaps(float, float, int, bool);
	void GenCylinder(float, float, int);
	void Rotate(XMMATRIX);
	void Translate(XMVECTOR);

	XMFLOAT3 GetPosition(int);
	XMFLOAT2 GetTexCoord(int);
	XMFLOAT3 GetNormal(int);

	int GetNumVertices();
	std::vector<unsigned int> GetIndices();

private:
	inline XMVECTOR GetCircleVector(int, int);

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
};