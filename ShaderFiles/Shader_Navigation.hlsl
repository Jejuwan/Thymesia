
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector			g_vColor = vector(0.f, 1.f, 0.f, 1.f);

struct VS_IN
{
	float3		vPosition : POSITION;	
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	return Out;
}

/* SV_POSITION에 해당하는 데이터의 W값으로 XYZW를 나눈다.(투영변환) */
/* 뷰포트 스페이스로 변환한다. (윈도우좌표로 변환한다.) */
/* 래스터라이즈를 수행한다. (픽셀을 생성한다.) */

struct PS_IN
{
	float4		vPosition : SV_POSITION;	
};

struct PS_OUT
{
	/* 0번째 렌더타겟에 그리는 픽셀의 색상이다. */
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_vColor;

	return Out;
}


technique11		DefaultTechnique
{
	pass Cell
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
}