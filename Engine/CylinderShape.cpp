
#include "CylinderShape.h"
//reference: https://github.com/Microsoft/DirectXTK/blob/master/Src/Geometry.cpp


CylinderShape::CylinderShape()
{

}

CylinderShape::CylinderShape(int radius, int height, int tesselation)
{
	GenCylinder(radius, height, tesselation);
}

CylinderShape::~CylinderShape()
{

}

inline XMVECTOR CylinderShape::GetCircleVector(int i, int t)
{
	float angle = (i * XM_2PI) / t;
	float deltaX;
	float deltaZ;

	XMScalarSinCos(&deltaX, &deltaZ, angle);
	
	return XMVectorSet(deltaX, 0, deltaZ, 0.f);
}

void CylinderShape::GenCaps(float r, float h, int t, bool isTop)
{
	for (int i = 0; i < t - 2; ++i)
	{
		int index1 = (i + 1) % t;
		int index2 = (i + 2) % t;

		if (isTop)
		{
			int temp = index1;
			index1 = index2;
			index2 = temp;
		}

		int verts = m_Vertices.size();
		m_Indices.push_back(verts);
		m_Indices.push_back(verts + index1);
		m_Indices.push_back(verts + index2);
	}

	XMVECTOR normal = g_XMIdentityR1; //UP
	XMVECTOR texScale = g_XMNegativeOneHalf;

	if (!isTop)
	{
		normal = XMVectorNegate(normal);
		texScale = XMVectorMultiply(texScale, g_XMNegateX);
	}

	for (int i = 0; i < t; ++i)
	{
		XMVECTOR circleVec = GetCircleVector(i, t);
		XMVECTOR position = XMVectorAdd(XMVectorScale(circleVec, r), XMVectorScale(normal, h));
		XMVECTOR texCoord = XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circleVec), texScale, g_XMOneHalf);
		Vertex v;
		XMStoreFloat3(&v.position, position);
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.texCoord, texCoord);
		m_Vertices.push_back(v);
	}
}

void CylinderShape::GenCylinder(float r, float h, int t)
{
	m_Vertices.clear();
	m_Indices.clear();

	float height = float(h) / 2;

	XMVECTOR offsetTop = XMVectorScale(g_XMIdentityR1, height);

	int stride = t + 1;

	for (int i = 0; i <= t; ++i)
	{
		XMVECTOR normal = GetCircleVector(i, t);
		XMVECTOR offsetSide = XMVectorScale(normal, r);

		float u = float(i) / t;

		XMVECTOR texCoord = XMLoadFloat(&u);
		Vertex vertex;
		XMStoreFloat3(&vertex.position, XMVectorAdd(offsetSide, offsetTop));
		XMStoreFloat3(&vertex.normal, normal);
		XMStoreFloat2(&vertex.texCoord, texCoord);
		m_Vertices.push_back(Vertex(vertex.position, vertex.normal, vertex.texCoord));

		XMStoreFloat3(&vertex.position, XMVectorSubtract(offsetSide, offsetTop));
		XMStoreFloat3(&vertex.normal, normal);
		XMStoreFloat2(&vertex.texCoord, XMVectorAdd(texCoord, g_XMIdentityR1));
		m_Vertices.push_back(Vertex(vertex.position, vertex.normal, vertex.texCoord));
		
		m_Indices.push_back(i * 2 + 1);
		m_Indices.push_back((i * 2 + 2) % (stride * 2));
		m_Indices.push_back(i * 2);

		m_Indices.push_back((i * 2 + 3) % (stride * 2));
		m_Indices.push_back((i * 2 + 2) % (stride * 2));
		m_Indices.push_back(i * 2 + 1);
	}

	GenCaps(r, height, t, true);
	GenCaps(r, height, t, false);


	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		m_Vertices[i].texCoord.x = (1.f - m_Vertices[i].texCoord.x);
	}
}

void CylinderShape::Rotate(XMMATRIX rotation)
{
	for (int i = 0; i < m_Vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_Vertices[i].position);
		XMVECTOR nPos = XMVector3Transform(pos, rotation);
		XMStoreFloat3(&m_Vertices[i].position, nPos);

		XMVECTOR norm = XMLoadFloat3(&m_Vertices[i].normal);
		XMVECTOR nNorm = XMVector3TransformNormal(norm, rotation);
		XMStoreFloat3(&m_Vertices[i].normal, nNorm);

		XMVECTOR tex = XMLoadFloat2(&m_Vertices[i].texCoord);
		XMVECTOR nTex = XMVector2Transform(tex, rotation);
		XMStoreFloat2(&m_Vertices[i].texCoord, nTex);		
	}
}

void CylinderShape::Translate(XMVECTOR dir)
{
	XMMATRIX trans = XMMatrixTranslationFromVector(dir);
	for (int i = 0; i < m_Vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_Vertices[i].position);
		XMVECTOR nPos = XMVector3Transform(pos, trans);
		XMStoreFloat3(&m_Vertices[i].position, nPos);
	}
}

XMFLOAT3 CylinderShape::GetPosition(int i)
{
	return m_Vertices[i].position;
}

XMFLOAT2 CylinderShape::GetTexCoord(int i)
{
	return m_Vertices[i].texCoord;
}

XMFLOAT3 CylinderShape::GetNormal(int i)
{
	return m_Vertices[i].normal;
}

int CylinderShape::GetNumVertices()
{
	return m_Vertices.size();
}

std::vector<unsigned int> CylinderShape::GetIndices()
{
	return m_Indices;
}