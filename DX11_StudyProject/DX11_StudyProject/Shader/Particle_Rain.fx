struct Particle
{
	float3 initialPosW	: POSITION;
	float3 initialVelW	: VELOCITY;
	float2 sizeW		: SIZE;
	float age			: AGE;
	uint type			: TYPE;
};

struct VertexOut
{
	float3 PosW	: POSITION;
	uint type	: TYPE;
};

VertexOut vs_main(Particle i)
{
	VertexOut o;

	float t = i.age;

	o.PosW = i.initialPosW + 0.5 * t * t * 
}