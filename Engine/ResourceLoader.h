#pragma once



#include <map>
#include <memory>
#include "textureclass.h"
#include "d3dclass.h"
#include "Structs.h"


class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();

	void Shutdown();

	void LoadTexture(ID3D11Device* device, TextureID textureId, WCHAR* filename);

	TextureClass& GetTexture(TextureID textureId);

	bool LoadModel(ModelID modelID, WCHAR* filename);
	ModelInfo& GetModel(ModelID modelId);


private:


	std::map<TextureID, std::unique_ptr<TextureClass>> m_TexturesMap;
	std::map<ModelID, std::unique_ptr<ModelInfo>> m_ModelMap;

};

