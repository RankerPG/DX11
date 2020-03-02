SamplerState smpLinear : register(s0);

TextureCube g_CubeMap : register(t0);

cbuffer cbData : register(b0)
{
	float4x4 g_matWVP;
}

struct VertexIn
{
	float4 pos : POSITION;
};

struct VertexOut
{
	float4 pos	: SV_POSITION;
	float4 posL	: POSITION;
};

VertexOut vs_main(VertexIn i)
{
	VertexOut o;

	o.pos = mul(i.pos, g_matWVP);

	o.posL = i.pos;

	return o;
}

float4 ps_main(VertexOut i) : SV_TARGET
{
	return g_CubeMap.Sample(smpLinear, i.posL.xyz);
}

DepthStencilState LessEqualDSS
{
	DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};