
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

vector			g_vCamPosition;
float4			g_Color;
float2			g_Size;

struct VS_IN
{
	float3		vPosition : POSITION0;
	float2		vTexcoord : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float4		vRotDir : POSITION1;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	//vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	vector vPosition = vector(In.vPosition, 1.f);
	matrix		matWV, matWVP;

	//vPosition = mul(vPosition, mat);

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

/* SV_POSITION에 해당하는 데이터의 W값으로 XYZW를 나눈다.(투영변환) */
/* 뷰포트 스페이스로 변환한다. (윈도우좌표로 변환한다.) */
/* 래스터라이즈를 수행한다. (픽셀을 생성한다.) */

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
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}