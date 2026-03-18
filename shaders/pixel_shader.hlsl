
Texture2D texDiffuse    : register(t0);
Texture2D texNormal     : register(t1);
Texture2D texSpec       : register(t2);

TextureCube cubeMapSkybox : register(t3);


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

SamplerState texSampler_Diffuse : register(s0);
SamplerState texSampler_Skybox : register(s1);

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
    
    float4 WorldPos : World_Position;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float3 GetNormals(PSIn input)
{
    float4 texture_normals = texNormal.Sample(texSampler_Diffuse, input.TexCoord);
    if (texture_normals.a < 1)
    {
        return input.Normal;

    }
        
    
    texture_normals = texture_normals * 2.0f - 1.0f; // to vector from color

    
    float3x3 TBN_mat = float3x3(input.Tangent, input.Binormal, input.Normal);
    
    return mul(float1x3(texture_normals.xyz), TBN_mat);
}





float4 SkyboxCubemap(float3 view)
{
    return cubeMapSkybox.Sample(texSampler_Diffuse, -view);
}

float4 ReflectionsCubemap(float3 normals, float3 view)
{
    float3 skyBoxSample = reflect(-view, normals);
    
    return cubeMapSkybox.Sample(texSampler_Diffuse, skyBoxSample);
}

float4 PS_Testing(PSIn input)
{
    float3 worldNormal = normalize(GetNormals(input));

    float3 view = normalize(input.WorldPos - cameraPos).xyz;

    
    
    
    return SkyboxCubemap(view);

}


float4 PS_HalfLambert(PSIn input)
{ // Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	
    
    float3 worldNormal = normalize(GetNormals(input));
    
    
    //if (input.Pos.x > 500)
    //    worldNormal = input.Normal;
   
    
    
  
    //return float4(worldNormal * 0.5 + 0.5, 1);
    
    //return specular_texture;
    
    float3 lightD = normalize((lightPos - input.WorldPos).xyz);
    
    float light_distance = distance(lightPos, input.WorldPos);
    
    float light_amount = pow(15.0 / light_distance, 10) - 0.01;
    light_amount = clamp(light_amount, -0.1, 1);
	
    float lambert = dot(worldNormal, lightD);
    float half_lambert = (lambert + 1) / 2;
    
    lambert *= light_amount;
    half_lambert *= light_amount;
    
    float3 reflectedLight = reflect(lightD, worldNormal);
    float3 view = normalize(input.WorldPos - cameraPos).xyz;
    
    
    
    float4 specular_texture = texSpec.Sample(texSampler_Diffuse, input.TexCoord);
    
    specular_texture = lerp(float4(1, 1, 1, 1), specular_texture, specular_texture.a);
    
    float specular = abs(clamp(dot(view, reflectedLight), 0, 1));
    
    specular = pow(specular, diffuseColor_Glossyness.a * 0.5) * light_amount;
    
    float bands = 1;
    
    float dither = sin((input.Pos.x + input.Pos.y)) * 0.1;
    
    

    float sin2 = sin((input.Pos.x + input.Pos.y) * 0.3);
    
    //return float4(dither, dither, dither, 1);
    
    float utalizedLambert = lerp(lambert, half_lambert, light_amount * 0.7);
    
    float posterized = smoothstep(0.099, 0.1, lambert + dither);
    
    //posterized = max(step(0, sin2), posterized);
    
	
    float4 ambient = float4(0.1, 0, 0.4, 0);
    float4 diffuse = float4(0.66, 0.6, 0.1, 1);
    
    
    
    
    
    //return input.WorldPos;
    //return float4(input.Normal * 0.5 + 0.5, 1) ;
    //return input.Pos;
    
    float4 specColor = float4(1, 1, 0.5, 0);
    
    float frensel = pow(abs((dot(worldNormal, view) + 1) / 2), 2);
    
    
    float4 diffuseTex = texDiffuse.Sample(texSampler_Diffuse, input.TexCoord);
    
    
    float4 lightColor = float4(0.8, 0.8, 0.3, 0.5);
    
    float4 reflectionColor = ReflectionsCubemap(worldNormal, view);
   
   
    // comp
    
    // reflection
    float4 reflection_component = reflectionColor * specular_texture;
    
    // diffuse
    
    float lerp_refl = 0.1;
    float4 diffuse_base = (float4(diffuseColor_Glossyness.yxz, 1) * diffuseTex) * (1 - lerp_refl) + reflection_component * lerp_refl;
    float4 diffuse_light = half_lambert * lightColor;
    
    
    
    float4 diffuse_component = diffuse_base * diffuse_light;
    
    // ambiant
    float4 ambiant_component = AmbiantColor;
    
    // specular
    float4 spec_base = specular_texture * reflectionColor;
    float4 spec_light = specular * lightColor * lightColor.a;
    float4 specular_component = spec_base * spec_light;
    
    // combination
    return diffuse_component + ambiant_component + specular_component;

    
    
    
    
}


float4 PS_Styalized(PSIn input)
{
    // Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	
    
    float3 worldNormal = normalize(GetNormals(input));
    
    
    //if (input.Pos.x > 500)
    //    worldNormal = input.Normal;
   
    
    
  
    //return float4(worldNormal * 0.5 + 0.5, 1);
    
    //return specular_texture;
    
    float3 lightD = normalize((lightPos - input.WorldPos).xyz);
    
    float light_distance = distance(lightPos, input.WorldPos);
    
    float light_amount = pow(15.0 / light_distance, 10) - 0.01;
    light_amount = clamp(light_amount, -0.1, 1);
	
    float lambert = dot(worldNormal, lightD);
    float half_lambert = (lambert + 1) / 2;
    
    lambert *= light_amount;
    half_lambert *= light_amount;
    
    float3 reflectedLight = reflect(lightD, worldNormal);
    float3 view = normalize(input.WorldPos - cameraPos).xyz;
    
    
    
    float4 specular_texture = texSpec.Sample(texSampler_Diffuse, input.TexCoord);
    
    specular_texture = lerp(float4(1, 1, 1, 1), specular_texture, specular_texture.a);
    
    float specular = abs(clamp(dot(view, reflectedLight), 0, 1));
    
    specular = pow(specular, diffuseColor_Glossyness.a * 0.5) * light_amount;
    
    float bands = 1;
    
    float dither = sin((input.Pos.x + input.Pos.y)) * 0.1;
    
    

    float sin2 = sin((input.Pos.x + input.Pos.y) * 0.3);
    
    //return float4(dither, dither, dither, 1);
    
    float utalizedLambert = lerp(lambert, half_lambert, light_amount * 0.7);
    
    float posterized = smoothstep(0.099, 0.1, lambert + dither);
    
    //posterized = max(step(0, sin2), posterized);
    
	
    float4 ambient = float4(0.1, 0, 0.4, 0);
    float4 diffuse = float4(0.66, 0.6, 0.1, 1);
    
    
    
    
    
    //return input.WorldPos;
    //return float4(input.Normal * 0.5 + 0.5, 1) ;
    //return input.Pos;
    
    float4 specColor = float4(1, 1, 0.5, 0);
    
    float frensel = pow(abs((dot(worldNormal, view) + 1) / 2), 2);
    
    
    float4 diffuseTex = texDiffuse.Sample(texSampler_Diffuse, input.TexCoord);
    
    
    float4 lightColor = float4(0.8, 0.8, 0.3, 0.5);
    
    
    

    
    float edge = step(0.1, frensel * half_lambert - dither * 0.1);
    
    float4 specularStep = smoothstep(0.49, 0.5, specular * specular_texture + dither * 1);
    

    return float4(diffuseColor_Glossyness.yxz, 1) * diffuseTex * (posterized * lightColor) + AmbiantColor + specularStep * specular_texture * lightColor * lightColor.a + edge * specularColor * lightColor; //frensel * 10 + step(0.4, frensel * half_lambert);
    
    
    
}




float4 PS_main(PSIn input) : SV_Target
{
    return PS_HalfLambert(input);

}

