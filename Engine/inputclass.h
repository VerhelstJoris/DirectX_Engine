////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////


#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#include "Structs.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();
	void Update();
	void LateUpdate();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);
	bool IsMouseButtonDown(unsigned int button);
	bool IsKeypressed(unsigned int);

	float2 GetMousePosition() { return m_CurrMousePosition; };
	float2 GetMouseMovement() { return m_MouseMovement; };


private:
	bool m_keys[256];
	bool m_PrevFrameKeys[256];

	float2 m_CurrMousePosition, m_PrevMousePosition, m_MouseMovement;

};

#endif