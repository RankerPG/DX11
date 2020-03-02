cbuffer cbFrame
{
	float4x4 g_ViewProj;
	float3 g_ViewPos;
	float3 g_EmitPosW;
	float3 g_EmitDirW;
	float g_GameTime;
	float g_DeltaTime;
};

cbuffer cbFixed
{
	float3 g_AccelW = { -1.0, -9.8, 0.f };
};

SamplerState smpLinear : register (s0);

Texture1D g_RandomTex : register(t0);

#define PT_EMITTER 0
#define PT_FLARE 1

struct Particle
{
	float3 initialPosW	: POSITION;
	float3 initialVelW	: VELOCITY;
	float2 sizeW		: SIZE;
	float age			: AGE;
	uint type			: TYPE;
};

float3 RandomPos(float offset)
{
	float u = g_GameTime + offset;

	float3 v = g_RandomTex.SampleLevel(smpLinear, u, 0).xyz;

	return normalize(v);
}

Particle vs_main(Particle i)
{
	return i;
}

[maxvertexcount(6)]
void StreamOutGS(point Particle gin[1], inout PointStream<Particle> ptStream)
{
	gin[0].age += g_DeltaTime;

	if (PT_EMITTER == gin[0].type)
	{
		if (gin[0].age > 0.002)
		{
			for (int i = 0; i < 5; ++i)
			{
				float3 vRandom = 35.0 * RandomPos((float)i+1 / 5.0);
				vRandom.y = 20.f;

				Particle p;
				p.initialPosW = g_EmitPosW + vRandom;
				p.initialVelW = 0.f;
				p.sizeW = 1.f;
				p.age = 0.f;
				p.type = PT_FLARE;

				ptStream.Append(p);
			}
		}

		gin[0].age = 0.f;

		ptStream.Append(gin[0]);
	}
	else
	{
		if (gin[0].age <= 3.f)
		{
			ptStream.Append(gin[0]);
		}
	}
}

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

GeometryShader gsStreamOut = ConstructGSWithSO(CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyzw; VELOCITY.xy; SIZE.xy; AGE.x; TYPE.x");

technique11 Tech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vs_main()));
		SetGeometryShader(gsStreamOut);
		SetPixelShader(NULL);
		SetDepthStencilState(DisableDepth, 0);
	}
};