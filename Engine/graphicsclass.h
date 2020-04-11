////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "GameObject.h"

#include "SkySphere.h"
#include "skydomeshaderclass.h"

#include "lightshaderclass.h"
#include "lightclass.h"

#include "particleshaderclass.h"
#include "particlesystemclass.h"
#include "ResourceLoader.h"


#include "RenderTexture.h"

//terrain
#include "terrainclass.h"
#include "terrainshaderclass.h"


//tree
#include "FractalTree.h"
#include "Line.h"
#include "colorshaderclass.h"
#include "orthowindowclass.h"
#include "PostProcessingShader.h"

#include "Structs.h"
#include "SelectEnum.h"
#include <vector>

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND,const GameContext& context, GraphicsScenes sceneSelect);
	void Shutdown();
	bool Frame(const GameContext& context);

private:
	bool Render(const GameContext& context);

	bool RenderToTexture(const GameContext& context);
	bool RenderScene(const GameContext& context);



private:
	D3DClass* m_D3D;
	std::vector<GameObject*> m_GameObjects;
	LightShaderClass* m_LightShader;			//standard gameobject shader
	
	LightClass* m_Light;
	SkySphere* m_Sky;
	SkyDomeShaderClass* m_SkyShader;			//shader for skysphere

	ParticleShaderClass* m_ParticleShader;		//shader for particles
	ParticleSystemClass* m_ParticleSystem;

	ResourceLoader m_ResourceLoader;

	GraphicsScenes m_SceneSelect;

	//terrain
	TerrainClass* m_Terrain;
	TerrainShaderClass* m_TerrainShader;

	//tree
	FractalTree* m_FractalTree, *m_ShowCaseTree;
	int m_ShowCaseRuleSetId = 0;
	int m_ShowCaseIterations = 0;
	int m_Points = 0;

	ColorShaderClass* m_ColorShader;		//shader for the lines

	//post-processing
	RenderTexture* m_RenderTexture;
	OrthoWindowClass* m_OrthoWindow;
	PostProcessingShader* m_PostProcessingShader;

};

#endif