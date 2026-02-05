
Texture2D texDiffuse : register(t0);

cbuffer LightCameraBuffer : register(b0)
{
    float4 cameraPos;
    float4 lightPos;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 diffuseColor_Glossyness;
    float4 specularColor;
    float4 AmbiantColor;
}

SamplerState texSampler : register(s0);

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
    
    float4 WorldPos : World_Position;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	
    float3 lightD = normalize((lightPos - input.WorldPos).xyz);
	
    float lambert = dot(input.Normal, lightD);
    float half_lambert = (lambert + 1) / 2;
    
    
    float3 reflectedLight = reflect(lightD, input.Normal);
    float3 view = normalize(input.WorldPos - cameraPos).xyz;
    
    float specular = abs(clamp(dot(view, reflectedLight), 0, 1));
    
    specular = pow(specular, diffuseColor_Glossyness.a);
    
    float bands = 1;
    
    float dither = sin((input.Pos.x + input.Pos.y)) * 0.1;
    
    

    float sin2 = sin((input.Pos.x + input.Pos.y) * 0.3);
    
    //return float4(dither, dither, dither, 1);
    
    float posterized = smoothstep(0.099, 0.1, lambert + dither );
    
    //posterized = max(step(0, sin2), posterized);
    
	
    float4 ambient = float4(0.1, 0, 0.4, 0);
    float4 diffuse = float4(0.66, 0.6, 0.1, 1);
    
    
    
    
    
    //return input.WorldPos;
    //return float4(input.Normal * 0.5 + 0.5, 1) ;
    //return input.Pos;
    
    float4 specColor = float4(1,1, 0.5, 0);
    
    float frensel = pow(abs((dot(input.Normal, view) + 1) / 2), 2);
    
    //return float4(diffuseColor_Glossyness.yxz, 1) * half_lambert + ambient + specular * specularColor;

    
    float edge = step(0.1, frensel * half_lambert - dither * 0.1);
    
    return float4(diffuseColor_Glossyness.yxz, 1) * posterized + AmbiantColor + step(0.1, specular + dither * 0.1) * specularColor + edge * specularColor; //frensel * 10 + step(0.4, frensel * half_lambert);
    
    //return float4(diffuseColor_Glossyness.yxz, 1);
    
    
    //return frensel;
    
    //return cameraPos;
    
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
    //return float4(input.TexCoord, 0, 1);
}

