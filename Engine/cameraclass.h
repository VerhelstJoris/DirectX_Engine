#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

#include "Structs.h"

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void Update(const GameContext& context);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();
	D3DXVECTOR3 GetUpVector();
	D3DXVECTOR3 GetRightVector();
	D3DXVECTOR3 GetForwardVector();

	void GetViewMatrix(D3DXMATRIX&);
	void GetPostProcessingViewMatrix(D3DXMATRIX&);
	void CreatePostProcessingViewMatrix();
	void Render();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix, m_postViewMatrix;
	D3DXVECTOR3 m_Up, m_Forward, m_Position;


	float m_MoveSpeed = 10.0f;
	float m_RotationSpeed =10.0f;

};

#endif