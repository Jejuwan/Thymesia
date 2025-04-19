
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

vector			g_vCamPosition;
float4			g_Color;
float2 g_Size;
float g_fAlpha;

struct VS_IN
{
	float3		vPosition : POSITION;
	row_major float4x4	TransformMatrix : WORLD;
	row_major float4x4	RotationMatrix : WORLD;
	float		vRotDir : PSIZE;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
    row_major float4x4 RotationMatrix : WORLD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	vector		vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);

	Out.RotationMatrix = float4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	if(g_Size.y - g_Size.x > 0.5f)
	{
		float4		vLook = float4(In.vPosition.x, In.vPosition.y, In.vPosition.z, 1) - float4(g_vCamPosition.x, g_vCamPosition.y, g_vCamPosition.z,1);
		//float fAngle = dot(vector(0, 1, 0, 0), vector(In.vRotDir.x, In.vRotDir.y, In.vRotDir.z, 0));
		
		float3 normalizedAxis = normalize(vector(0,0,1,0));
		float cosAngle = cos(0.85f);
		float sinAngle = sin(0.85f);
		float oneMinusCosAngle = 1.0 - cosAngle;

		Out.RotationMatrix = float4x4(
			cosAngle + normalizedAxis.x * normalizedAxis.x * oneMinusCosAngle,
			normalizedAxis.x * normalizedAxis.y * oneMinusCosAngle - normalizedAxis.z * sinAngle,
			normalizedAxis.x * normalizedAxis.z * oneMinusCosAngle + normalizedAxis.y * sinAngle,
			0.0,

			normalizedAxis.y * normalizedAxis.x * oneMinusCosAngle + normalizedAxis.z * sinAngle,
			cosAngle + normalizedAxis.y * normalizedAxis.y * oneMinusCosAngle,
			normalizedAxis.y * normalizedAxis.z * oneMinusCosAngle - normalizedAxis.x * sinAngle,
			0.0,

			normalizedAxis.z * normalizedAxis.x * oneMinusCosAngle - normalizedAxis.y * sinAngle,
			normalizedAxis.z * normalizedAxis.y * oneMinusCosAngle + normalizedAxis.x * sinAngle,
			cosAngle + normalizedAxis.z * normalizedAxis.z * oneMinusCosAngle,
			0.0,

			0.0, 0.0, 0.0, 1.0
			);
	}


	
	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
    row_major float4x4 RotationMatrix : WORLD;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT			Out[4];

	float4		vLook = g_vCamPosition - In[0].vPosition;

    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * g_Size.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight)) * g_Size.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);


	matVP = mul(In[0].RotationMatrix, matVP);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexcoord = float2(0.f, 0.f);

	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexcoord = float2(1.f, 0.f);

	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexcoord = float2(1.f, 1.f);

	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexcoord = float2(0.f, 1.f);

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();	
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	/* 0번째 렌더타겟에 그리는 픽셀의 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a < 0.3f)
		discard;

	Out.vColor.rgba = g_Color;

	return Out;
}


technique11		DefaultTechnique
{
	pass Particle
	{
		SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}