
/////////////
// GLOBALS //
/////////////
Texture2D diffuseTexture;
Texture2D normalTexture;

SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	float3 reflection;
	float4 specular;

	float4 normalMap;
	float3 normal;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = diffuseTexture.Sample(SampleType, input.tex);

	// Set the default output color to the ambient light value for all pixels.
	color = ambientColor;

	// Initialize the specular color.
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sample the pixel in the bump map.
	normalMap = normalTexture.Sample(SampleType, input.tex);

	// Expand the range of the normal value from (0, +1) to (-1, +1).
	normalMap = (normalMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	normal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);

	// Normalize the resulting bump normal.
	normal = normalize(normal);

	// Invert the light direction for calculations.
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		color += (diffuseColor * lightIntensity);

		// Saturate the ambient and diffuse color.
		color = saturate(color);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
		reflection = normalize(2 * lightIntensity * normal - lightDir);

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

	// Multiply the texture pixel and the input color to get the textured result.
	color = color * textureColor;

	// Add the specular component last to the output color.
	color = saturate(color + specular);

	return color;
}