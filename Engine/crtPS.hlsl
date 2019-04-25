////////////////////////////////////////////////////////////////////////////////
// Filename: texture.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer DataBuffer
{
	float amountOfColumns;
	float vignetteStrength;
	float speed;
	float distortionStrength;

	float time;
	float3 padding;
}


float4 crtPixelShader(PixelInputType input) : SV_TARGET
{
	//Distorting the screen
	//--------------------------------------------------------------------
	float2 uv = input.tex - float2(0.5f, 0.5f);
	uv = uv * distortionStrength*(1.0f / distortionStrength +  2.0f* uv.x * uv.x * uv.y * uv.y);
	uv += float2(0.5f, 0.5f);

	if (uv.x < 0 || uv.y < 0 || uv.x >1 || uv.y > 1)
	{
		return float4(0, 0, 0, 1);
	}


	//COLOURED LINES
	//--------------------------------------------------------------------
	int row = (uv.x + (time/ (amountOfColumns*speed) ) ) * amountOfColumns;
	//int row = input.tex.x * 256;

	// Pick up the color at the current position.
	// Add 0.1 to make black areas dark grey.
	float4 color = shaderTexture.Sample(SampleType, uv);
	float4 multiplier = float4(0, 0, 0, 1);
	float4 outColor = float4(0, 0, 0, 1);

	// Set two variables for overspill from the primary colors to the other colors.
	float spillOver1 = 0.1;
	float spillOver2 = 0.2;

	int row_check = (int)row% 3;

	if (row_check == 0)
	{
		//blue
		color.r *= 0.2;
		color.g *= 0.2;

		multiplier.b = 1; 
		multiplier.r = spillOver1; 
		multiplier.g = spillOver2;
	}
	else if (row_check == 1)
	{
		//green
		color.r *= 0.2;
		color.b *= 0.2;

		multiplier.g = 1;
		multiplier.b = spillOver1;
		multiplier.r = spillOver2;
	
	}
	else if (row_check == 2)
	{
		//red
		color.b *= 0.2;
		color.g *= 0.2;

		multiplier.r = 1;
		multiplier.g = spillOver1;
		multiplier.b = spillOver2;
	}

	outColor = color * multiplier;

	// Increase contrast.
	float contrast = 1.1f;
	color = (color - contrast * (color - 1.0) * color *(color - 0.5)) * 1.7f;

	//vignette around the borders
	//========================================
	float vigAmt = vignetteStrength + 0.3f*sin( 4.0f *cos(4.0f));
	float vignette = (1 - vigAmt * (uv.y - 0.5f)*(uv.y - 0.5f))*(1 - vigAmt * (uv.x - 0.5f)*(uv.x - 0.5f));
	color *= vignette;


	// The Alpha channel needs to be restored to one after all operations.
	color.a = 1;

	return color;

}

