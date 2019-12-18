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

Texture2D g_DiffuseTex : register (t0);

cbuffer cbData : register (b0)
{
	float4x4 g_ViewProj;
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

struct VertexIn
{
	float4 posW		: POSITION;
	float2 sizeW	: SIZE;
};

struct VertexOut
{
	float3 centerW	: POSITION;
	float2 sizeW	: SIZE;
};

struct GeoOut
{
	float4 pos	: SV_POSITION;
	float4 posW : POSITION;
	float3 nrmW	: NORMAL;
	float2 uv	: TEXCOORD;
	uint primID	: SV_PrimitiveID;
};

VertexOut vs_main(VertexIn i)
{
	VertexOut o;

	o.centerW = i.posW.xyz;

	o.sizeW = i.sizeW;

	return o;
}

[maxvertexcount(4)]
void gs_main(point VertexOut gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
	float3 up = float3(0.f, 1.f, 0.f);
	float3 look = g_ViewPos - gin[0].centerW;
	look.y = 0.f;
	look = normalize(look);
	float3 right = cross(up, look);

	float halfWidth = 0.5f * gin[0].sizeW.x;
	float halfHeight = 0.5f * gin[0].sizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].centerW + halfWidth * right - halfHeight * up, 1.f);
	v[1] = float4(gin[0].centerW + halfWidth * right + halfHeight * up, 1.f);
	v[2] = float4(gin[0].centerW - halfWidth * right - halfHeight * up, 1.f);
	v[3] = float4(gin[0].centerW - halfWidth * right + halfHeight * up, 1.f);

	float2 uv[4] =
	{
		float2(0.f, 1.f),
		float2(0.f, 0.f),
		float2(1.f, 1.f),
		float2(1.f, 0.f)
	};

	GeoOut o;

	for (int i = 0; i < 4; ++i)
	{
		o.pos = mul(v[i], g_ViewProj);
		o.posW = v[i];
		o.nrmW = look;
		o.uv = uv[i];
		o.primID = primID;

		triStream.Append(o);
	}
}

void ComputeDirectionalLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);
void ComputePointLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);

float4 ps_main(GeoOut i) : SV_Target
{
	float4 diff = 0;
	float4 amb = 0;
	float4 spec = 0;

	int index = i.primID % 4;

	float4 texColor = g_DiffuseTex.Sample(smpLinear, i.uv);

	clip(texColor.a - 0.1f);

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
		SetGeometryShader(CompileShader(gs_5_0, gs_main()));
		SetPixelShader(CompileShader(ps_5_0, ps_main()));
	}
};