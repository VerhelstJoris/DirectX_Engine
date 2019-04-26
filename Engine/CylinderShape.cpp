
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

void CylinderShape::GenCaps(float radius, float height, int tesselation, bool isTop)
{

	for (int i = 0; i < tesselation - 2; ++i)
	{
		int index1 = (i + 1) % tesselation;
		int index2 = (i + 2) % tesselation;

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

	for (int i = 0; i < tesselation; ++i)
	{
		XMVECTOR circleVec = GetCircleVector(i, tesselation);
		XMVECTOR position = XMVectorAdd(XMVectorScale(circleVec, radius), XMVectorScale(normal, height));
		XMVECTOR texCoord = XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circleVec), texScale, g_XMOneHalf);
		Vertex v;
		XMStoreFloat3(&v.position, position);
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.texCoord, texCoord);
		m_Vertices.push_back(v);
	}
}

void CylinderShape::GenCylinder(float radius, float height, int tesselation)
{
	m_Vertices.clear();
	m_Indices.clear();


	XMVECTOR offsetTop = XMVECTOR{0,height,0,1};

	int stride = tesselation + 1;

	for (int i = 0; i <= tesselation; ++i)
	{
		XMVECTOR normal = GetCircleVector(i, tesselation);
		XMVECTOR offsetSide = XMVectorScale(normal, radius);

		float u = float(i) / tesselation;

		XMVECTOR texCoord = XMLoadFloat(&u);
		Vertex vertex;
		XMStoreFloat3(&vertex.position, XMVectorAdd(offsetSide, offsetTop));
		XMStoreFloat3(&vertex.normal, normal);
		XMStoreFloat2(&vertex.texCoord, texCoord);
		m_Vertices.push_back(Vertex(vertex.position, vertex.normal, vertex.texCoord));

		XMStoreFloat3(&vertex.position, offsetSide);
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

	GenCaps(radius, height, tesselation, true);
	//GenCaps(radius, height, tesselation, false);


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

