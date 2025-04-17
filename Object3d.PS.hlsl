#include "Object3d.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//表面の材質
struct Material
{
	float32_t4 color;
};

//コンスタントバッファの定義
//b = constantBuffer,0 = shader上でのresourceナンバー
ConstantBuffer<Material>gMaterial : register(b0);

struct PixcelShaderOutput
{
	float32_t4 color : SV_TARGET0;
};


PixcelShaderOutput main(VertexShaderOutput input)
{
	float32_t4 textureColor = gTexture.Sample(gSampler,input.texcoord);

	PixcelShaderOutput output;
    output.color = gMaterial.color * textureColor;
	
	
	return output;

}

