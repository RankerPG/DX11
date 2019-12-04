struct DirLight
{
	float3 dir;
	float4 diff;
	float4 amb;
	float4 spec;
};

struct Material
{
	float4 diff;
	float4 amb;
	float4 spec; // w = power
};

cbuffer cbData : register (b0)
{
	float4x4 g_matWorld;
	float4x4 g_matWorldInvT;
	float4x4 g_matWVP;
};

cbuffer cbLight : register (b1)
{
	DirLight g_Light;
};

cbuffer cbMtrl : register (b2)
{
	Material g_Mtrl;
};

cbuffer cbFrame : register (b3)
{
	float3 g_ViewPos;
}

struct VertexIn
{
	float4 pos : POSITION;
	float3 nrm : NORMAL;
};

struct VertexOut
{
	float4 pos	: SV_POSITION;
	float4 posW : POSITION;
	float3 nrmW	: NORMAL;
};

VertexOut vs_main(VertexIn i)
{
	VertexOut o;

	o.pos = mul(i.pos, g_matWVP);

	o.posW = mul(i.pos, g_matWorld);

	float3 nrmW = mul(i.nrm, (float3x3)g_matWorldInvT);
	
	o.nrmW = nrmW;

	return o;
}

float4 ps_main(VertexOut i) : SV_Target
{
	float4 diff = max(dot(g_Light.dir, i.nrmW), 0) * g_Light.diff * g_Mtrl.diff;
	float4 amb = g_Light.amb * g_Mtrl.amb;
	float3 R = normalize(reflect(-g_Light.dir, i.nrmW));
	float3 V = normalize(g_ViewPos - i.posW.xyz);
	float spec = pow(max(dot(R, V), 0), g_Mtrl.spec.w);

	float4 col = diff + amb + spec;

	return col;
}

technique11 ColorTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};