////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

#include <d3dx10math.h>


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	int i;
	

	// Initialize all the keys to being released and not pressed.
	for(i=0; i<256; i++)
	{
		m_keys[i] = false;
		m_PrevFrameKeys[i] = false;
	}


	//mouse
	m_CurrMousePosition = {0.0f,0.0f};
	m_PrevMousePosition = { 0.0f,0.0f };
	m_MouseMovement = { 0.0f,0.0f };

	return;
}

void InputClass::Update()
{
	m_PrevMousePosition = m_CurrMousePosition;
	POINT temp;
	GetCursorPos(&temp);
	m_CurrMousePosition.x = temp.x;
	m_CurrMousePosition.y = temp.y;

	m_MouseMovement.x = m_CurrMousePosition.x - m_PrevMousePosition.x;
	m_MouseMovement.y = m_CurrMousePosition.y - m_PrevMousePosition.y;

}

void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

bool InputClass::IsMouseButtonDown(unsigned int button)
{
	
	return (GetKeyState(button) & 0x100) != 0;
	
}

bool InputClass::IsKeypressed(unsigned int button)
{
	if (m_keys[button] && !m_PrevFrameKeys[button])
	{
		return true;
	}

	return false;
}

void InputClass::LateUpdate()
{
	for (size_t i = 0; i < 256; i++)
	{
		m_PrevFrameKeys[i] = m_keys[i];
	}
}