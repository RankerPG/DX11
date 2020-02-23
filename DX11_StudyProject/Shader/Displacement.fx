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
	float4 Env;
};

SamplerState smpLinear : register (s0);

Texture2D g_DiffuseTex : register (t0);

Texture2D g_NormalMapTex : register (t1);

cbuffer cbData : register (b0)
{
	float4x4 g_matWorld;
	float4x4 g_matWorldInvT;
	float4x4 g_matViewProj;
	float4x4 g_matTex;
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
	float4 g_FogColor;
	float g_FogStart;
	float g_FogRange;
}

cbuffer cbLight : register (b4)
{
	PointLight g_PointLight;
};

cbuffer cbTess : register (b5)
{
	float g_MaxTessDistance;
	float g_MinTessDistance;
	float g_MaxTessFactor;
	float g_MinTessFactor;
	float4 g_HeightScale;
}

struct VertexIn
{
	float4 pos	: POSITION;
	float3 nrm	: NORMAL;
	float3 tan	: TANGENT;
	float2 uv	: TEXCOORD;
};

struct VertexOut
{
	float4 posW			: POSITION;
	float3 nrmW			: NORMAL;
	float3 tanW			: TANGENT;
	float2 uv			: TEXCOORD;
	float tessFactor	: TESS;
};

VertexOut vs_main(VertexIn i)
{
	VertexOut o;

	o.posW = mul(i.pos, g_matWorld);

	o.nrmW = mul(i.nrm, (float3x3)g_matWorldInvT);

	o.tanW = mul(i.tan, (float3x3)g_matWorldInvT);

	o.uv = mul(float4(i.uv, 0.f, 1.f), g_matTex).xy;

	float D = length(o.posW - float4(g_ViewPos, 1.f));

	float tess = saturate((g_MinTessDistance - D) / (g_MinTessDistance - g_MaxTessDistance));

	o.tessFactor = g_MinTessFactor + tess * (g_MaxTessFactor - g_MinTessFactor);

	o.tessFactor -= frac(o.tessFactor);

	return o;
}

struct PatchTess
{
	float edgeTess[3]	: SV_TessFactor;
	float insideTess	: SV_InsideTessFactor;
};

PatchTess PatchHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess o;

	o.edgeTess[0] = 0.5f * (patch[1].tessFactor + patch[2].tessFactor);
	o.edgeTess[1] = 0.5f * (patch[2].tessFactor + patch[0].tessFactor);
	o.edgeTess[2] = 0.5f * (patch[0].tessFactor + patch[1].tessFactor);

	o.insideTess = o.edgeTess[0];

	return o;
}

struct HullOut
{
	float4 posW : POSITION;
	float3 nrmW : NORMAL;
	float3 tanW : TANGENT;
	float2 uv	: TEXCOORD;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HullOut HS(InputPatch<VertexOut, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HullOut o;
	
	o.posW = p[i].posW;
	o.nrmW = p[i].nrmW;
	o.tanW = p[i].tanW;
	o.uv = p[i].uv;

	return o;
}

struct DomainOut
{
	float4 pos	: SV_POSITION;
	float4 posW : POSITION;
	float3 nrmW : NORMAL;
	float3 tanW : TANGENT;
	float2 uv	: TEXCOORD;
};

[domain("tri")]
DomainOut DS(PatchTess patchTess, float3 bary : SV_DomainLocation, const OutputPatch<HullOut, 3> tri)
{
	DomainOut o;

	o.posW = bary.x * tri[0].posW + bary.y * tri[1].posW + bary.z * tri[2].posW;
	o.nrmW = bary.x * tri[0].nrmW + bary.y * tri[1].nrmW + bary.z * tri[2].nrmW;
	o.tanW = bary.x * tri[0].tanW + bary.y * tri[1].tanW + bary.z * tri[2].tanW;
	o.uv = bary.x * tri[0].uv + bary.y * tri[1].uv + bary.z * tri[2].uv;

	o.nrmW = normalize(o.nrmW);

	const float MipInterval = 20.f;

	float mipLevel = clamp((length(o.posW - float4(g_ViewPos, 1.f)) - MipInterval) / MipInterval, 0.f, 6.f);

	float2 uv = frac(o.uv);

	float h = g_NormalMapTex.SampleLevel(smpLinear, uv, mipLevel).a;

	o.posW.xyz += (g_HeightScale.x * (h - 1)) * o.nrmW;

	o.pos = mul(o.posW, g_matViewProj);

	return o;
}

void ComputeDirectionalLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);
void ComputePointLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);

float4 ps_main(DomainOut i) : SV_Target
{
	float4 diff = 0;
	float4 amb = 0;
	float4 spec = 0;

	float4 texColor = g_DiffuseTex.Sample(smpLinear, i.uv);

	clip(texColor.a - 0.1f);

	float4 D, A, S;

	float3 N = normalize(i.nrmW);
	float3 T = normalize(i.tanW - dot(i.tanW, N) * N);
	float3 B = cross(N, T);

	float3 tangentN = g_NormalMapTex.Sample(smpLinear, i.uv).xyz;

	tangentN = tangentN * 2.f - 1.f;

	float3x3 TBN = float3x3(T, B, N);

	N = mul(tangentN, TBN);

	ComputeDirectionalLight(i.posW.xyz, N, D, A, S);

	diff += D;
	amb += A;
	spec += S;

	ComputePointLight(i.posW.xyz, N, D, A, S);

	diff += D;
	amb += A;
	spec += S;

	float3 eyePos = g_ViewPos - i.posW.xyz;

	float d = length(eyePos);

	eyePos /= d;

	float fogLerp = saturate((d - g_FogStart) / g_FogRange);

	float4 litColor = texColor * (diff + amb) + spec;

	float4 col = lerp(litColor, g_FogColor, fogLerp);

	col.a = texColor.a * g_Mtrl.diff.a;

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

	diff = diffFactor * g_PointLight.diff * g_Mtrl.diff;
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
		SetHullShader(CompileShader(hs_5_0, HS()));
		SetDomainShader(CompileShader(ds_5_0, DS()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};