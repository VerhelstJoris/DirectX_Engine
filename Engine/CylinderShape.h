#pragma once

#include <directxmath.h>
#include <vector>

using namespace DirectX;


//TO-DO
//MAKE SIMILAR CLASSES FOR OTHER BASIC SHAPES

class CylinderShape
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
	CylinderShape();
	CylinderShape(int, int, int);
	~CylinderShape();

	void GenCaps(float, float, int, bool);
	void GenCylinder(float, float, int);
	void Rotate(XMMATRIX);
	void Translate(XMVECTOR);

	XMFLOAT3 GetPosition(int i){return m_Vertices[i].position;}
	XMFLOAT2 GetTexCoord(int i){return m_Vertices[i].texCoord;}
	XMFLOAT3 GetNormal(int i){return m_Vertices[i].normal;}

	int GetNumVertices(){return m_Vertices.size();}
	std::vector<unsigned int> GetIndices(){return m_Indices;}

private:
	inline XMVECTOR GetCircleVector(int, int);

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
};