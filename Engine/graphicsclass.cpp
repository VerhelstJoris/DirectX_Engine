////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "inputclass.h"

#include <iostream>

//#define TERRAIN
//#define GRAPHICS
#define PROCEDURAL

GraphicsClass::GraphicsClass() 
{
	m_D3D = 0;
	m_GameObjects.clear();
	m_LightShader = 0;
	m_Light = 0;

	m_ParticleShader = 0;
	m_ParticleSystem = 0;
	m_SkyShader = 0;
	m_Sky = 0;

	m_RenderTexture = 0;
	m_PostProcessingShader = 0;
	m_OrthoWindow = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
	
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, const GameContext& context)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	context.camera->SetPosition(-13.75, 7.5f, 13.25f);
	context.camera->SetRotation(15.0f, 150.0f, 0.0f);

	context.camera->Render();
	context.camera->GetViewMatrix(baseViewMatrix);

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetPosition(5.0, 10.0, 5.0);
	m_Light->SetSpecularPower(32.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTexture();
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	// Create the full screen ortho window object.
	m_OrthoWindow = new OrthoWindowClass;
	if (!m_OrthoWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_OrthoWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_PostProcessingShader = new PostProcessingShader();
	if (!m_PostProcessingShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_PostProcessingShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_PostProcessingShader->Toggle();

#pragma region RESOURCELOADING
	//initialize the resource loader
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_HOUSE, L"../Engine/data/Textures/T_house.jpg");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_HOUSE, L"../Engine/data/Textures/N_house.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_DOOR, L"../Engine/data/Textures/T_Door.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_DOOR, L"../Engine/data/Textures/N_Door.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_SMALLPROPS, L"../Engine/data/Textures/T_SmallProps.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_SMALLPROPS, L"../Engine/data/Textures/N_SmallProps.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_GROUND, L"../Engine/data/Textures/T_Ground.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_GROUND, L"../Engine/data/Textures/N_Ground.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_WHEEL, L"../Engine/data/Textures/T_Wheel.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_WHEEL, L"../Engine/data/Textures/N_Wheel.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_WINDOW, L"../Engine/data/Textures/T_Window.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_WINDOW, L"../Engine/data/Textures/N_Window.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_TREE, L"../Engine/data/Textures/T_Tree.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_TREE, L"../Engine/data/Textures/N_Tree.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_PIERPLATFORM, L"../Engine/data/Textures/T_PierPlatform.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_PIERPLATFORM, L"../Engine/data/Textures/N_PierPlatform.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_PIERPILLAR, L"../Engine/data/Textures/T_PierPilars.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_PIERPILLAR, L"../Engine/data/Textures/N_PierPilars.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_ROCKS1, L"../Engine/data/Textures/T_Rocks1.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_ROCKS1, L"../Engine/data/Textures/N_Rocks1.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_ROCKS2, L"../Engine/data/Textures/T_Rocks2.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_ROCKS2, L"../Engine/data/Textures/N_Rocks2.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_ROCKPILE, L"../Engine/data/Textures/T_RockPile.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_ROCKPILE, L"../Engine/data/Textures/N_RockPile.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_SEAWEED, L"../Engine/data/Textures/T_Seaweed.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::N_SEAWEED, L"../Engine/data/Textures/N_Seaweed.BMP");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_SKY, L"../Engine/data/Textures/T_Sky.png");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_WOOD, L"../Engine/data/Textures/T_Wood.png");
	m_ResourceLoader.LoadTexture(m_D3D->GetDevice(), TextureID::T_LEAF, L"../Engine/data/Textures/T_Leaf.png");


	m_ResourceLoader.LoadModel(ModelID::HOUSE, L"../Engine/data/Models/house.txt");
	m_ResourceLoader.LoadModel(ModelID::DOOR, L"../Engine/data/Models/door.txt");
	m_ResourceLoader.LoadModel(ModelID::SIGN, L"../Engine/data/Models/sign.txt");
	m_ResourceLoader.LoadModel(ModelID::GROUND, L"../Engine/data/Models/ground.txt");
	m_ResourceLoader.LoadModel(ModelID::WHEEL, L"../Engine/data/Models/wheel.txt");
	m_ResourceLoader.LoadModel(ModelID::WINDOW, L"../Engine/data/Models/window.txt");
	m_ResourceLoader.LoadModel(ModelID::BARREL, L"../Engine/data/Models/barrel.txt");
	m_ResourceLoader.LoadModel(ModelID::TREE, L"../Engine/data/Models/tree.txt");
	m_ResourceLoader.LoadModel(ModelID::PIERPLATFORM, L"../Engine/data/Models/pierplatform.txt");
	m_ResourceLoader.LoadModel(ModelID::PIERPILLAR1, L"../Engine/data/Models/pierPillar1.txt");
	m_ResourceLoader.LoadModel(ModelID::PIERPILLAR2, L"../Engine/data/Models/pierPillar2.txt");
	m_ResourceLoader.LoadModel(ModelID::CHIMNEY, L"../Engine/data/Models/chimney.txt");
	m_ResourceLoader.LoadModel(ModelID::CRATE, L"../Engine/data/Models/crate.txt");
	m_ResourceLoader.LoadModel(ModelID::ROCKS1, L"../Engine/data/Models/rocks1.txt");
	m_ResourceLoader.LoadModel(ModelID::ROCKS2, L"../Engine/data/Models/rocks2.txt");
	m_ResourceLoader.LoadModel(ModelID::SEAWEED1, L"../Engine/data/Models/seaweed1.txt");
	m_ResourceLoader.LoadModel(ModelID::SEAWEED2, L"../Engine/data/Models/seaweed2.txt");
	m_ResourceLoader.LoadModel(ModelID::ROCKPILE, L"../Engine/data/Models/rockpile.txt");
	m_ResourceLoader.LoadModel(ModelID::PLANK1, L"../Engine/data/Models/plank1.txt");
	m_ResourceLoader.LoadModel(ModelID::PLANK2, L"../Engine/data/Models/plank2.txt");
	m_ResourceLoader.LoadModel(ModelID::BUOY1, L"../Engine/data/Models/buoy1.txt");
	m_ResourceLoader.LoadModel(ModelID::BUOY2, L"../Engine/data/Models/buoy2.txt");
	m_ResourceLoader.LoadModel(ModelID::SPHERE, L"../Engine/data/Models/sphere.txt");


#pragma endregion

#ifdef  GRAPHICS



	

	#pragma region GAMEOBJECTS
	// Create the models
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::HOUSE),  &m_ResourceLoader.GetTexture(TextureID::T_HOUSE) , &m_ResourceLoader.GetTexture(TextureID::N_HOUSE)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::DOOR),   &m_ResourceLoader.GetTexture(TextureID::T_DOOR), &m_ResourceLoader.GetTexture(TextureID::N_DOOR)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::SIGN),   &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::GROUND), &m_ResourceLoader.GetTexture(TextureID::T_GROUND), &m_ResourceLoader.GetTexture(TextureID::N_GROUND)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::WHEEL),  &m_ResourceLoader.GetTexture(TextureID::T_WHEEL), &m_ResourceLoader.GetTexture(TextureID::N_WHEEL)));

	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::WINDOW),  &m_ResourceLoader.GetTexture(TextureID::T_WINDOW), &m_ResourceLoader.GetTexture(TextureID::N_WINDOW)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::WINDOW),  &m_ResourceLoader.GetTexture(TextureID::T_WINDOW), &m_ResourceLoader.GetTexture(TextureID::N_WINDOW)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BARREL),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BARREL),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BARREL),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));

	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::TREE),  &m_ResourceLoader.GetTexture(TextureID::T_TREE), &m_ResourceLoader.GetTexture(TextureID::N_TREE)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PIERPLATFORM),  &m_ResourceLoader.GetTexture(TextureID::T_PIERPLATFORM), &m_ResourceLoader.GetTexture(TextureID::N_PIERPLATFORM)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PIERPILLAR1),  &m_ResourceLoader.GetTexture(TextureID::T_PIERPILLAR), &m_ResourceLoader.GetTexture(TextureID::N_PIERPILLAR)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PIERPILLAR2),  &m_ResourceLoader.GetTexture(TextureID::T_PIERPILLAR), &m_ResourceLoader.GetTexture(TextureID::N_PIERPILLAR)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::CHIMNEY),  &m_ResourceLoader.GetTexture(TextureID::T_PIERPILLAR), &m_ResourceLoader.GetTexture(TextureID::N_PIERPILLAR)));

	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::CRATE),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::CRATE),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::CRATE),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::CRATE),  &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKS1), &m_ResourceLoader.GetTexture(TextureID::T_ROCKS1), &m_ResourceLoader.GetTexture(TextureID::N_ROCKS1),false));

	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKS1),  &m_ResourceLoader.GetTexture(TextureID::T_ROCKS1), &m_ResourceLoader.GetTexture(TextureID::N_ROCKS1), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKS1),  &m_ResourceLoader.GetTexture(TextureID::T_ROCKS1), &m_ResourceLoader.GetTexture(TextureID::N_ROCKS1), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKS2),  &m_ResourceLoader.GetTexture(TextureID::T_ROCKS2), &m_ResourceLoader.GetTexture(TextureID::N_ROCKS2), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKS2),  &m_ResourceLoader.GetTexture(TextureID::T_ROCKS2), &m_ResourceLoader.GetTexture(TextureID::N_ROCKS2), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::SEAWEED1),  &m_ResourceLoader.GetTexture(TextureID::T_SEAWEED), &m_ResourceLoader.GetTexture(TextureID::N_SEAWEED), false));
	
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::SEAWEED1), &m_ResourceLoader.GetTexture(TextureID::T_SEAWEED), &m_ResourceLoader.GetTexture(TextureID::N_SEAWEED), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::SEAWEED2), &m_ResourceLoader.GetTexture(TextureID::T_SEAWEED), &m_ResourceLoader.GetTexture(TextureID::N_SEAWEED), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::SEAWEED2), &m_ResourceLoader.GetTexture(TextureID::T_SEAWEED), &m_ResourceLoader.GetTexture(TextureID::N_SEAWEED)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::T_ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::N_ROCKPILE), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::T_ROCKPILE),& m_ResourceLoader.GetTexture(TextureID::N_ROCKPILE), false));

	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::T_ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::N_ROCKPILE), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::T_ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::N_ROCKPILE), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::T_ROCKPILE), &m_ResourceLoader.GetTexture(TextureID::N_ROCKPILE), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PLANK1), &m_ResourceLoader.GetTexture(TextureID::T_PIERPLATFORM), &m_ResourceLoader.GetTexture(TextureID::N_PIERPLATFORM)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PLANK2), &m_ResourceLoader.GetTexture(TextureID::T_PIERPLATFORM), &m_ResourceLoader.GetTexture(TextureID::N_PIERPLATFORM), false));
														 
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PLANK2), &m_ResourceLoader.GetTexture(TextureID::T_PIERPLATFORM), &m_ResourceLoader.GetTexture(TextureID::N_PIERPLATFORM), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::PLANK2), &m_ResourceLoader.GetTexture(TextureID::T_PIERPLATFORM), &m_ResourceLoader.GetTexture(TextureID::N_PIERPLATFORM), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BUOY1), &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS)));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BUOY2), &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS), false));
	m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(),&m_ResourceLoader.GetModel(ModelID::BUOY2), &m_ResourceLoader.GetTexture(TextureID::T_SMALLPROPS), &m_ResourceLoader.GetTexture(TextureID::N_SMALLPROPS), false));

	//m_GameObjects.push_back(new GameObject(m_D3D->GetDevice(), &m_ResourceLoader.GetModel(ModelID::SPHERE), &m_ResourceLoader.GetTexture(TextureID::T_SKY), &m_ResourceLoader.GetTexture(TextureID::N_SEAWEED), false));

	for (size_t i = 0; i < m_GameObjects.size(); i++)
	{
		if (!m_GameObjects[i])
		{
			return false;
		}
	}

	#pragma endregion

	#pragma region MODEL TRANSFORMATIONS

	//window 1
	m_GameObjects[5]->SetPosition(-4, 4.16, 2);
	m_GameObjects[5]->SetRotation(15,-148, -8.5);

	//window 2
	m_GameObjects[6]->SetPosition(-1, 2.89, 0.22);
	m_GameObjects[6]->SetRotation(5 , -148 , 5.3);

	//barrel 1
	m_GameObjects[7]->SetPosition(0.2, 0.72, 0.82);
	m_GameObjects[7]->SetRotation(0, 95 , 0);

	//barrel 2
	m_GameObjects[8]->SetPosition(1.1, 0.72, -0.1);
	m_GameObjects[8]->SetRotation(0, -95, 0);

	//barrel 3
	m_GameObjects[9]->SetPosition(-7.39, 0.68, -3.42);
	m_GameObjects[9]->SetRotation(-5 , -19 , -5 );

	//crate 1
	m_GameObjects[15]->SetPosition(-9.43, 0.72, -3.42);
	m_GameObjects[15]->SetRotation(0.8, 95, 3.5);

	//crate 2
	m_GameObjects[16]->SetPosition(-4.25, 0.72, 3.2);
	m_GameObjects[16]->SetRotation(0.8, 0.8, -3.5);

	//crate 3
	m_GameObjects[17]->SetPosition(-3, 0.72, 2.85);
	m_GameObjects[17]->SetRotation(1.2, 15, -3.5);

	//crate 4
	m_GameObjects[18]->SetPosition(-3, 1.06, 2.75);
	m_GameObjects[18]->SetRotation(1.2, 35, -3.5);

	//rocks 1
	m_GameObjects[19]->SetScale(0.01, 0.0075, 0.01);
	m_GameObjects[19]->SetPosition(5.5, 1.87, 6.5);
	m_GameObjects[19]->SetRotation(0, -100, 0);

	//rocks 2
	m_GameObjects[20]->SetScale(0.0125, 0.01, 0.01);
	m_GameObjects[20]->SetPosition(5.25, 1.87, -3.75);
	m_GameObjects[20]->SetRotation(0, -50, 0);

	//rocks 3
	m_GameObjects[21]->SetScale(0.01, 0.0075, 0.01);
	m_GameObjects[21]->SetPosition(6.25, 1.02, 0.35);
	m_GameObjects[21]->SetRotation(0, -75, 0);

	//rocks 4
	m_GameObjects[22]->SetScale(0.015, 0.015, 0.0125);
	m_GameObjects[22]->SetPosition(6.55, 0.4, -1.29);
	m_GameObjects[22]->SetRotation(-5, 85, 0);

	//rocks 5
	m_GameObjects[23]->SetScale(0.015, 0.01, 0.01);
	m_GameObjects[23]->SetPosition(-7.9, -0.1, -11);
	m_GameObjects[23]->SetRotation(-10, 210, 0);

	//seaweed 1 
	m_GameObjects[24]->SetScale(0.0125, 0.015, 0.0125);
	m_GameObjects[24]->SetPosition(6.75, 0.6, 7.75);
	m_GameObjects[24]->SetRotation(0, -134, 0);

	//seaweed 2
	m_GameObjects[25]->SetScale(0.015, 0.015, 0.015);
	m_GameObjects[25]->SetPosition(-10.6, 0.75, -8.5);
	m_GameObjects[25]->SetRotation(0, 90, 0);

	//seaweed 3
	m_GameObjects[26]->SetScale(0.0125, 0.0125, 0.0125);
	m_GameObjects[26]->SetPosition(-10.7, 0.76, -7.55);
	m_GameObjects[26]->SetRotation(0, 95, 0);

	//seaweed 4
	m_GameObjects[27]->SetPosition(-8.9, 5.71, -1);
	m_GameObjects[27]->SetRotation(0, 134, 0);

	//rockPile 1
	m_GameObjects[28]->SetScale(0.0275, 0.0275 , 0.0275);
	m_GameObjects[28]->SetPosition(-11.25, 0.63, 2.25);
	m_GameObjects[28]->SetRotation(0, 165, 0);

	//rockPile 2
	m_GameObjects[29]->SetScale(0.0125, 0.0125, 0.0125);
	m_GameObjects[29]->SetPosition(-9.25, 0.69, 5.25);
	m_GameObjects[29]->SetRotation(0, -50, 0);

	//rockPile 3
	m_GameObjects[30]->SetScale(0.0125, 0.01, 0.0125);
	m_GameObjects[30]->SetPosition(0, 0.71, 5);
	m_GameObjects[30]->SetRotation(0, 70, 0);

	//rockPile 4
	m_GameObjects[31]->SetScale(0.0275, 0.0275, 0.0275);
	m_GameObjects[31]->SetPosition(3.5, 0.63, 6);
	m_GameObjects[31]->SetRotation(0, -50, 0);

	//rockPile 5
	m_GameObjects[32]->SetScale(0.0425, 0.0425, 0.0425);
	m_GameObjects[32]->SetPosition(4.5, 0.63, -0.5);
	m_GameObjects[32]->SetRotation(0, -95, 0);

	//plank 1 
	m_GameObjects[34]->SetScale(0.01, 0.0075, 0.01);
	m_GameObjects[34]->SetPosition(-6.45, 0.81, 5.4);
	m_GameObjects[34]->SetRotation(-85, 0, 80);

	//plank 2
	m_GameObjects[35]->SetScale(0.0125, 0.0125, 0.0115);
	m_GameObjects[35]->SetPosition(-12.5, 1.5, 0.5);
	m_GameObjects[35]->SetRotation(167, 142, -2.8);

	//plank 3
	m_GameObjects[36]->SetScale(0.0125, 0.01, 0.01);
	m_GameObjects[36]->SetPosition(-11.25, 1.36, -1.25);
	m_GameObjects[36]->SetRotation(-4.6, -46, 2.9);

	//buoy 1
	m_GameObjects[38]->SetScale(0.01, 0.01, 0.01);
	m_GameObjects[38]->SetPosition(-5.5, 2.43, 3.5);
	m_GameObjects[38]->SetRotation(0, -154, 0);

	//buoy 2
	m_GameObjects[39]->SetScale(0.0125, 0.015, 0.0125);
	m_GameObjects[39]->SetPosition(-1.5, 3.89, 0.8);
	m_GameObjects[39]->SetRotation(5, -154, 0);

	#pragma endregion


	m_Sky = new SkySphere(&m_ResourceLoader.GetModel(ModelID::SPHERE), D3DXVECTOR4(0.0,0.3,0.39,1), D3DXVECTOR4(0.21, 0.14, 0.15, 1));
	if (!m_Sky)
	{
		return false;
	}

	m_Sky->SetPosition(0, 20.0, 0);

	// Initialize the sky dome object.
	result = m_Sky->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky object.", L"Error", MB_OK);
		return false;
	}

	// Create the sky dome shader object.
	m_SkyShader = new SkyDomeShaderClass;
	if (!m_SkyShader)
	{
		return false;
	}

	// Initialize the sky dome shader object.
	result = m_SkyShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky dome shader object.", L"Error", MB_OK);
		return false;
	}



	
	// Create the particle shader object.
	m_ParticleShader = new ParticleShaderClass;
	if (!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the particle shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the particle system object.
	m_ParticleSystem = new ParticleSystemClass;
	if (!m_ParticleSystem)
	{
		return false;
	}

	// Initialize the particle system object.
	result = m_ParticleSystem->Initialize(m_D3D->GetDevice(), L"../Engine/data/Textures/cloud.png");
	if (!result)
	{
		return false;
	}

	m_ParticleSystem->SetPosition(0.8, 8.75, -1);

#endif //  GRAPHICS


#ifdef TERRAIN



	//NEW TERRAIN TEMPORARY SETUP
	//============================
	m_Terrain = new TerrainClass();
	if (!m_Terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	//result = m_Terrain->Initialize(m_D3D->GetDevice(), "../Engine/data/heightmap01.bmp");
	result = m_Terrain->InitializeTerrain(m_D3D->GetDevice(), 128, 128);   //initialise the flat terrain.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}
	else
	{
		//m_Terrain->GenerateHeightMap(m_D3D->GetDevice(), true);
		m_Terrain->GeneratePerlinHeightMap(m_D3D->GetDevice(), true);
	}

	// Create the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

#endif // TERRAIN

#ifdef PROCEDURAL
	m_FractalTree = new FractalTree(m_D3D->GetDevice(), XMVECTOR{ 0.0f,0.0f,0.0f });
	if (!m_FractalTree)
	{
		return false;
	}

	m_FractalTree->Generate();
	m_FractalTree->SetBranchDiffuseTexture(&m_ResourceLoader.GetTexture(TextureID::T_WOOD));
	m_FractalTree->SetBranchNormalTexture(&m_ResourceLoader.GetTexture(TextureID::N_SEAWEED));
	m_FractalTree->SetLeafDiffuseTexture(&m_ResourceLoader.GetTexture(TextureID::T_LEAF));
	m_FractalTree->SetLeafNormalTexture(&m_ResourceLoader.GetTexture(TextureID::N_SEAWEED));

	result = m_FractalTree->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Fractal Tree.", L"Error", MB_OK);
		return false;
	}
	
#endif // TREE


	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_PostProcessingShader)
	{
		m_PostProcessingShader->Shutdown();
		delete m_PostProcessingShader;
		m_PostProcessingShader = 0;
	}

#ifdef GRAPHICS
	

	// Release the model object.
	for (size_t i = 0; i < m_GameObjects.size(); i++)
	{
		m_GameObjects[i]->Shutdown();
		delete m_GameObjects[i];
		m_GameObjects[i] = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the particle system object.
	if (m_ParticleSystem)
	{
		m_ParticleSystem->Shutdown();
		delete m_ParticleSystem;
		m_ParticleSystem = 0;
	}

	// Release the particle shader object.
	if (m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		delete m_ParticleShader;
		m_ParticleShader = 0;
	}

	m_ResourceLoader.Shutdown();


		// Release the sky dome shader object.
	if (m_SkyShader)
	{
		m_SkyShader->Shutdown();
		delete m_SkyShader;
		m_SkyShader = 0;
	}

	// Release the sky dome object.
	if (m_Sky)
	{
		m_Sky->Shutdown();
		delete m_Sky;
		m_Sky = 0;
	}

#endif // GRAPHICS

#ifdef TERRAIN

	// Release the terrain shader object.
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

#endif // TERRAIN


#ifdef PROCEDURAL

	if (m_FractalTree)
	{
		m_FractalTree->ShutDown();
		delete m_FractalTree;
		m_FractalTree = 0;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	//if (m_DebugWindow)
	//{
	//	m_DebugWindow->Shutdown();
	//	delete m_DebugWindow;
	//	m_DebugWindow = 0;
	//}

	if (m_OrthoWindow)
	{
		m_OrthoWindow->Shutdown();
		delete m_OrthoWindow;
		m_OrthoWindow = 0;
	}
#endif // TREE


	return;
}

bool GraphicsClass::Frame(const GameContext& context)
{

	bool result;

#ifdef GRAPHICS

	// Run the frame processing for the particle system.
	m_ParticleSystem->Frame(context, m_D3D->GetDeviceContext());


#endif // GRAPHICS

	// Render the graphics scene.
	result = Render(context);
	if(!result)
	{
		return false;
	}

	//postProcessing TOGGLES
	if (context.input->IsKeyDown(VK_NUMPAD8))
	{
		m_PostProcessingShader->IncrementCount(true);
	}

	if (context.input->IsKeyDown(VK_NUMPAD7))
	{
		m_PostProcessingShader->IncrementCount(false);
	}

	if (context.input->IsKeyDown(VK_NUMPAD5))
	{
		m_PostProcessingShader->IncrementSpeed(false);
	}

	if (context.input->IsKeyDown(VK_NUMPAD4))
	{
		m_PostProcessingShader->IncrementSpeed(true);
	}

	if (context.input->IsKeyDown(VK_NUMPAD1))
	{
		m_PostProcessingShader->IncrementBrightness(false);
	}
	if (context.input->IsKeyDown(VK_NUMPAD2))
	{
		m_PostProcessingShader->IncrementBrightness(true);
	}

	if (context.input->IsKeyDown(VK_NUMPAD3))
	{
		m_PostProcessingShader->IncrementDistortion(false);
	}
	if (context.input->IsKeyDown(VK_NUMPAD6))
	{
		m_PostProcessingShader->IncrementDistortion(true);
	}

	if (context.input->IsKeypressed(VK_NUMPAD0))
	{
		m_PostProcessingShader->Toggle();
	}

	m_PostProcessingShader->UpdateTimer(context.deltaTime);

#ifdef TERRAIN

	if (context.input->IsKeyDown(VK_SPACE))
	{
		m_Terrain->GenerateHeightMap(m_D3D->GetDevice(),true);
	}

#endif // TERRAIN


#ifdef PROCEDURAL

	if (context.input->IsKeypressed(VK_SPACE))
	{
		m_FractalTree->SetBranchLength(m_FractalTree->GetBranchLength()+0.5f);
		m_FractalTree->Generate();
		m_FractalTree->Initialize();
	}

	if (context.input->IsKeypressed('N'))
	{
		m_FractalTree->NextRuleSet();
	}


#endif 

	return true;
}

bool GraphicsClass::Render(const GameContext& context)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, postViewMatrix;
	bool result;

	//render entire scene to the texture
	if (m_PostProcessingShader->GetToggledOn())
	{
		result = RenderToTexture(context);
		if (!result)
		{
			return false;
		}
	}

	//MODELS
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.1f, 0.1f, 0.5f, 1.0f);
	
	result = RenderScene(context);
	if (!result)
	{
		return false;
	}

	//2D rendering

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	context.camera->GetViewMatrix(viewMatrix);
	context.camera->GetPostProcessingViewMatrix(postViewMatrix);

	m_D3D->TurnZBufferOff();

	// Render the window using the texture shader.
	if (m_PostProcessingShader->GetToggledOn())
	{
		m_OrthoWindow->Render(m_D3D->GetDeviceContext());

		result = m_PostProcessingShader->Render(m_D3D->GetDeviceContext(), m_OrthoWindow->GetIndexCount(), worldMatrix, postViewMatrix,
			orthoMatrix, m_RenderTexture->GetShaderResourceView());
		if (!result)
		{
			return false;
		}
	}

	m_D3D->TurnZBufferOn();


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderScene(const GameContext& context)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	context.camera->Render();

	context.camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);


#ifdef GRAPHICS


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (size_t i = 0; i < m_GameObjects.size(); i++)
	{
		m_GameObjects[i]->Render(m_D3D->GetDeviceContext());

		// Render the model using the light shader.
		m_GameObjects[i]->GetWorldMatrix(worldMatrix);

		ID3D11ShaderResourceView* textureArr[2];
		textureArr[0] = m_GameObjects[i]->GetDiffuseTexture();
		textureArr[1] = m_GameObjects[i]->GetNormalTexture();

		result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_GameObjects[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
			textureArr,m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), context.camera->GetPosition(),
			m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	}

	//SKYSPHERE
	m_D3D->TurnOffFrontCulling();

	m_Sky->Render(m_D3D->GetDeviceContext());

	result = m_SkyShader->Render(m_D3D->GetDeviceContext(), m_Sky->GetIndexCount(), m_Sky->GetWorldMatrix(), viewMatrix, projectionMatrix,
		m_Sky->GetCenterColor(), m_Sky->GetApexColor());

	m_D3D->TurnOnFrontCulling();


	//PARTICLES
	m_D3D->EnableAlphaBlending();
	m_D3D->GetWorldMatrix(worldMatrix);

	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->Render(m_D3D->GetDeviceContext());

	// Render the model using the texture shader.
	result = m_ParticleShader->Render(m_D3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_ParticleSystem->GetTexture());
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending.
	m_D3D->DisableAlphaBlending();


#endif // GRAPHICS

#ifdef TERRAIN


	//TERRAIN
	// Render the terrain buffers.
	m_Terrain->Render(m_D3D->GetDeviceContext());

	// Render the terrain using the terrain shader.
	result = m_TerrainShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection());
	if (!result)
	{
		return false;
	}

#endif // TERRAIN

#ifdef PROCEDURAL
	//m_FractalTree->Render(m_D3D->GetDeviceContext());

	for (SimpleObject* object : m_FractalTree->GetBranchModels())
	{
	
		object->Render(m_D3D->GetDeviceContext());

		ID3D11ShaderResourceView* textureArr[2];
		textureArr[0] = m_FractalTree->GetBranchDiffuseTexture();
		textureArr[1] = m_FractalTree->GetBranchNormalTexture();

		result = m_LightShader->Render(m_D3D->GetDeviceContext(), object->GetIndexCount(), *reinterpret_cast<D3DXMATRIX*>(&object->GetWorldMatrix())
			, viewMatrix, projectionMatrix,
		textureArr, m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), context.camera->GetPosition(),
		m_Light->GetSpecularColor(), m_Light->GetSpecularPower());	
	}

	for (SimpleObject* object : m_FractalTree->GetLeafModels())
	{

		object->Render(m_D3D->GetDeviceContext());

		ID3D11ShaderResourceView* textureArr[2];
		textureArr[0] = m_FractalTree->GetLeafDiffuseTexture();
		textureArr[1] = m_FractalTree->GetLeafNormalTexture();

		result = m_LightShader->Render(m_D3D->GetDeviceContext(), object->GetIndexCount(), *reinterpret_cast<D3DXMATRIX*>(&object->GetWorldMatrix())
			, viewMatrix, projectionMatrix,
			textureArr, m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), context.camera->GetPosition(),
			m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	}
	
#endif // TREE

	//std::cout << "scene rendered" << std::endl;
	return true;
}

bool GraphicsClass::RenderToTexture(const GameContext& context)
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene(context);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}
