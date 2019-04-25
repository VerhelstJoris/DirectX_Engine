#include <DirectXMath.h>
#include <d3d11.h>

#include "CylinderShape.h"
#include "textureclass.h"

using namespace DirectX;



//TO-DO
//MAKE GAMEOBJECT CLASS INHERIT FROM THIS
//=======================================

class SimpleObject
{

public:
	struct ModelData
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

protected:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
		XMFLOAT3 normal;
	};	

public:
	SimpleObject();
	SimpleObject(CylinderShape&);
	SimpleObject(const SimpleObject&);
	~SimpleObject();

	bool InitializeBuffers(ID3D11Device*);
	void Render(ID3D11DeviceContext*) const;

	int GetIndexCount();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();

	void SetTexture(TextureClass* texture) { m_Texture = texture; };
	TextureClass* GetTexture() { return m_Texture; };

	void SetModelData(ModelData*);
	void SetVertexCount(int);
	void SetIndexCount(int);

	void SetPosition(XMVECTOR pos) { m_DebugPosition = pos; };
	XMVECTOR GetPosition() { return m_DebugPosition; };

protected:
	virtual void drawCurrent(ID3D11DeviceContext*) const;

private:
	void ReleaseBuffers();

	void ReleaseModel();

private:
	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	int m_VertexCount, m_IndexCount;
	ModelData* m_Model;
	std::vector<unsigned int> m_Indices;

	XMMATRIX m_WorldMatrix;
	XMVECTOR m_DebugPosition;
	TextureClass* m_Texture;
};
