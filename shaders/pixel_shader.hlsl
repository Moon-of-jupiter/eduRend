
Texture2D texDiffuse : register(t0);

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	
    float3 lightD = float3(0.5, 0.5, 0);
	
    float lambert = dot(input.Normal, normalize(lightD));
    float half_lambert = (lambert + 1) / 2;
    
    float bands = 4;
	
    float posterized = half_lambert;
    
    posterized *= bands;
    posterized = floor(posterized);
    posterized /= bands;
	
    float4 ambient = float4(0.2, 0, 0.2, 0);
    float4 diffuse = float4(0.66, 0.6, 0.1, 1);
    
    
	
    //return float4(input.Normal * 0.5 + 0.5, 1) ;
    //return input.Pos;
    return diffuse * posterized + ambient;
	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
    //return float4(input.TexCoord, 0, 1);
}