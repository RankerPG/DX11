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

cbuffer cbData : register (b0)
{
	float4x4 g_matWorld;
	float4x4 g_matWorldInvT;
	float4x4 g_matWVP;
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

VertexIn vs_main(VertexIn i)
{
	return i;
}

void Subdivide(VertexIn inVtx[3], out VertexIn outVtx[6])
{
	VertexIn m[3];

	m[0].pos = 0.5f * (inVtx[0].pos + inVtx[1].pos);
	m[1].pos = 0.5f * (inVtx[1].pos + inVtx[2].pos);
	m[2].pos = 0.5f * (inVtx[2].pos + inVtx[0].pos);

	m[0].pos = normalize(m[0].pos);
	m[1].pos = normalize(m[1].pos);
	m[2].pos = normalize(m[2].pos);

	m[0].nrm = normalize(inVtx[0].nrm + inVtx[1].nrm);
	m[1].nrm = normalize(inVtx[1].nrm + inVtx[2].nrm);
	m[2].nrm = normalize(inVtx[2].nrm + inVtx[0].nrm);

	m[0].uv = 0.5f * (inVtx[0].uv + inVtx[1].uv);
	m[1].uv = 0.5f * (inVtx[1].uv + inVtx[2].uv);
	m[2].uv = 0.5f * (inVtx[2].uv + inVtx[0].uv);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	outVtx[0] = inVtx[0];
	outVtx[1] = m[0];
	outVtx[2] = m[2];
	outVtx[3] = m[1];
	outVtx[4] = inVtx[2];
	outVtx[5] = inVtx[1];
}

void OutputSubdivision(VertexIn v[6], inout TriangleStream<VertexOut> triStream)
{
	VertexOut o[6];

	for (int i = 0; i < 6; ++i)
	{
		o[i].pos = mul(v[i].pos, g_matWVP);
		o[i].posW = mul(v[i].pos, g_matWorld);
		o[i].nrmW = mul(v[i].nrm, (float3x3)g_matWorldInvT);
		o[i].uv = mul(float4(v[i].uv, 0.f, 1.f), g_matTex).xy;
	}

	for (int j = 0; j < 5; ++j)
	{
		triStream.Append(o[j]);
	}

	triStream.RestartStrip();

	triStream.Append(o[1]);
	triStream.Append(o[5]);
	triStream.Append(o[3]);
}

[maxvertexcount(20)]
void gs_main(triangle VertexIn i[3], inout TriangleStream<VertexOut> triStream)
{
	VertexIn v[6];
	Subdivide(i, v);
	OutputSubdivision(v, triStream);
}

void ComputeDirectionalLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);
void ComputePointLight(float3 pos, float3 nrm, out float4 diff, out float4 amb, out float4 spec);

float4 ps_main(VertexOut i) : SV_Target
{
	float4 diff = 0;
	float4 amb = 0;
	float4 spec = 0;

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