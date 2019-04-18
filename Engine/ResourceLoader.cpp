#include "ResourceLoader.h"
#include <cassert>

#include <fstream>
using namespace std;


ResourceLoader::ResourceLoader()
{
}


ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::Shutdown()
{
	for (size_t i = 0; i < (int)TextureID::AMOUNTOFTEXTURES -1; i++)
	{
		//m_TexturesMap.find((TextureID)i)->second->Shutdown();
		m_TexturesMap.erase((TextureID)i);
	}
	

	//for (std::map<TextureID, std::unique_ptr<TextureClass>>::iterator it = m_TexturesMap.begin(); it != m_TexturesMap.end(); ++it)
	//{
	//
	//	if (it->second!=nullptr)
	//	{
	//		//it->second->Shutdown();
	//		m_TexturesMap.erase(it);
	//	}
	//}


	//
	for (size_t i = 0; i < (int)ModelID::AMOUNTOFMODELS - 1; i++)
	{
		//delete m_ModelMap.find((ModelID)i)->second->modelType;
		//m_ModelMap.find((ModelID)i)->second->modelType = 0;
	
		//m_ModelMap.erase((ModelID)i);
	}
}


void ResourceLoader::LoadTexture(ID3D11Device* device, TextureID textureId, WCHAR* filename)
{
	std::unique_ptr<TextureClass> texture(new TextureClass());
	texture->Initialize(device, filename);

	auto inserted = m_TexturesMap.insert(std::make_pair(textureId, std::move(texture)));
	assert(inserted.second);
}


TextureClass& ResourceLoader::GetTexture(TextureID textureId)
{
	const auto found = m_TexturesMap.find(textureId);
	assert(found != m_TexturesMap.end());
	return *found->second;
}

bool ResourceLoader::LoadModel(ModelID modelID, WCHAR* filename)
{

	ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	int vertexCount;
	fin >> vertexCount;

	// Set the number of indices to be the same as the vertex count.
	int indexCount = vertexCount;

	// Create the model using the vertex count that was read in.
	ModelType* model = new ModelType[vertexCount];
	if (!model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < vertexCount; i++)
	{
		fin >> model[i].x >> model[i].y >> model[i].z;
		fin >> model[i].tu >> model[i].tv;
		fin >> model[i].nx >> model[i].ny >> model[i].nz;
	}

	// Close the model file.
	fin.close();

	std::unique_ptr<ModelInfo> modelstr(new ModelInfo());

	modelstr->indexCount = indexCount;
	modelstr->vertexCount = vertexCount;
	modelstr->modelType = model;

	auto inserted = m_ModelMap.insert(std::make_pair(modelID, std::move(modelstr)));
	assert(inserted.second);
}


ModelInfo& ResourceLoader::GetModel(ModelID modelId)
{
	const auto found = m_ModelMap.find(modelId);
	assert(found != m_ModelMap.end());
	return *found->second;
}

