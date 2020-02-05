struct Material
{
	float4 diff;
	float4 amb;
	float4 spec; // w = power
	float4 Env;
};

cbuffer cbData : register (b0)
{
	float4x4 g_matWVP;
};

cbuffer cbMtrl : register (b1)
{
	Material g_Mtrl;
};

struct VertexIn
{
	float4 pos : POSITION;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut o;

	o.pos = mul(vin.pos, g_matWVP);

	return o;
}

float4 ps_main(VertexOut pin) : SV_Target
{
	return g_Mtrl.diff;
}

technique11 ColorTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};