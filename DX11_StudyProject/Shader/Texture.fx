struct DirLight
{
	float3 dir;
	float4 diff;
	float4 amb;
	float4 spec;
};

struct PointLight
{
	float3 pos;
	float range;
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

SamplerState smpLinear : register (s0);

Texture2D g_DiffuseTex;

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

cbuffer cbLight : register (b4)
{
	PointLight g_PointLight;
};

struct VertexIn
{
	float4 pos	: POSITION;
	float3 nrm	: NORMAL;
	float2 uv	: TEXCOORD;
};

struct VertexOut
{
	float4 pos	: SV_POSITION;
	float4 posW : POSITION;
	float3 nrmW	: NORMAL;
	float2 uv	: TEXCOORD;
};

VertexOut vs_main(VertexIn i)
{
	VertexOut o;

	o.pos = mul(i.pos, g_matWVP);

	o.posW = mul(i.pos, g_matWorld);

	float3 nrmW = mul(i.nrm, (float3x3)g_matWorldInvT);

	o.nrmW = nrmW;

	o.uv = i.uv;

	return o;
}

void ComputeDirectionalLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);
void ComputePointLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);

float4 ps_main(VertexOut i) : SV_Target
{
	float4 diff = 0;
	float4 amb = 0;
	float4 spec = 0;

	float4 texColor = g_DiffuseTex.Sample(smpLinear, i.uv);

	float4 D, A, S;

	float3 N = normalize(i.nrmW);

	ComputeDirectionalLight(i.posW.xyz, N, D, A, S);

	diff += D;
	amb += A;
	spec += S;

	ComputePointLight(i.posW.xyz, N, D, A, S);

	diff += D;
	amb += A;
	spec += S;

	float4 col = texColor * (diff + amb) + spec;

	return col;
}

void ComputeDirectionalLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec)
{
	float diffFactor = max(dot(g_Light.dir, nrm), 0);
	float specFactor = 0;

	if (diffFactor > 0.f)
	{
		float3 R = normalize(reflect(-g_Light.dir, nrm));
		float3 V = normalize(g_ViewPos - pos);

		specFactor = pow(max(dot(R, V), 0), g_Mtrl.spec.w);
	}

	diff = diffFactor * g_Light.diff * g_Mtrl.diff;
	amb = g_Light.amb * g_Mtrl.amb;
	spec = specFactor * g_Light.spec * g_Mtrl.spec;
}

void ComputePointLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec)
{
	float d = length(g_PointLight.pos - pos);

	if (g_PointLight.range < d)
		return;

	float3 litDir = normalize(g_PointLight.pos - pos);

	float diffFactor = max(dot(litDir, nrm), 0);
	float specFactor = 0;

	if (diffFactor > 0.f)
	{
		float3 R = normalize(reflect(-litDir, nrm));
		float3 V = normalize(g_ViewPos - pos);

		specFactor = pow(max(dot(R, V), 0), g_Mtrl.spec.w);
	}

	float att = 0.5f;

	diff = diffFactor * g_PointLight.diff;
	amb = g_PointLight.amb * g_Mtrl.amb;
	spec = 0; //specFactor* g_PointLight.spec * att;

	diff.rgb *= att;
	//spec.rgb *= att;
}

technique11 ColorTech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};