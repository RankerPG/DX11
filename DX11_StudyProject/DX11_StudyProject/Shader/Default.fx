cbuffer cbData
{
	float4x4 g_matWVP;
};

struct VertexIn
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut o;

	o.pos = mul(vin.pos, g_matWVP);

	o.col = vin.col;

	return o;
}

float4 ps_main(VertexOut pin) : SV_Target
{
	return pin.col;
}

technique11 ColorTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};