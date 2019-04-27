#pragma once


class CameraClass;
class InputClass;


struct GameContext
{
public:
	CameraClass* camera;
	InputClass* input;
	float deltaTime;
};

struct ModelType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};


struct float2
{
	float x;
	float y;
};

struct float3
{
	float x;
	float y;
	float z;
};

struct float4
{
	float x;
	float y;
	float z;
	float w;
};

static float Deg2Rad(float deg)
{
	return deg * 3.14159265 / 180;
}


enum class TextureID
{
	T_HOUSE,
	T_DOOR,
	T_SMALLPROPS,
	T_GROUND,
	T_WHEEL,
	T_WINDOW,
	T_BARREL,
	T_TREE,
	T_PIERPLATFORM,
	T_PIERPILLAR,
	T_ROCKS1,
	T_ROCKS2,
	T_SEAWEED,
	T_ROCKPILE,
	T_SKY,
	T_WOOD,
	T_LEAF,

	N_HOUSE,
	N_DOOR,
	N_SMALLPROPS,
	N_GROUND,
	N_WHEEL,
	N_WINDOW,
	N_PIERPLATFORM,
	N_PIERPILLAR,
	N_TREE,
	N_ROCKS1,
	N_ROCKS2,
	N_SEAWEED,
	N_ROCKPILE,


	AMOUNTOFTEXTURES
};



enum class ModelID
{
	HOUSE,
	DOOR,
	SIGN,
	GROUND,
	WHEEL,
	WINDOW,
	BARREL,
	TREE,
	PIERPLATFORM,
	PIERPILLAR1,
	PIERPILLAR2,
	CHIMNEY,
	CRATE,
	ROCKS1,
	ROCKS2,
	SEAWEED1,
	SEAWEED2,
	ROCKPILE,
	PLANK1,
	PLANK2,
	BUOY1,
	BUOY2,
	SPHERE,

	AMOUNTOFMODELS
};


struct ModelInfo
{
	ModelType* modelType;
	int vertexCount;
	int indexCount;
};