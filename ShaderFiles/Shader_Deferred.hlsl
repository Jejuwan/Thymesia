
#include "Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ProjMatrixInv, g_ViewMatrixInv;
matrix          g_LightViewProjMatrix;

texture2D		g_Texture;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;

vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vCamPosition;

vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;
texture2D		g_RimTexture;
texture2D		g_ShadowTexture;

float g_fFadeAlpha;


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
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

struct PS_OUT_LIGHT
{
	/* 0번째 렌더타겟에 그리는 픽셀의 색상이다. */
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

float3 ShadowFactor(float3 posWorld)
{
	// Shadow map
    float shadowFactor = 1.0;

    //const float nearZ = 0.01; // 카메라 설정과 동일
        
        // 1. Project posWorld to light screen    
    float4 lightScreen = mul(float4(posWorld, 1.0), g_LightViewProjMatrix);
    lightScreen.xyz /= lightScreen.w;
        
        // 2. 카메라(광원)에서 볼 때의 텍스춰 좌표 계산
    float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
    lightTexcoord += 1.0;
    lightTexcoord *= 0.5;
        
        // 3. 쉐도우맵에서 값 가져오기
    float depth = g_ShadowTexture.Sample(LinearSampler, lightTexcoord).r;
        
        // 4. 가려져 있다면 그림자로 표시
    if (depth + 0.001 < lightScreen.z)
        shadowFactor = 0.0;
	
    return shadowFactor;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	float4		vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 300.f;

	Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
	
	
	vector		vPosition;

	/* 화면에 그려지는 픽셀의 투영공간상의 위치를 구한다. */
	vPosition.x = In.vTexcoord.x * 2.f - 1.f;
	vPosition.y = In.vTexcoord.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.x;
	vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자 .*/
	vPosition = vPosition * fViewZ;
	vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구하자 .*/
	vPosition = mul(vPosition, g_ViewMatrixInv);

	vector		vLook = vPosition - g_vCamPosition;

	vector		vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
	
	
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) *
		pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 15.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
	float4		vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vPosition;

	/* 화면에 그려지는 픽셀의 투영공간상의 위치를 구한다. */
	vPosition.x = In.vTexcoord.x * 2.f - 1.f;
	vPosition.y = In.vTexcoord.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.x;
	vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자 .*/
	vPosition = vPosition * fViewZ;
	vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치를 구하자 .*/
	vPosition = mul(vPosition, g_ViewMatrixInv);

	vector		vLightDir = vPosition - g_vLightPos;

	float		fDistance = length(vLightDir);

	// 0.0 ~ 1.f
	float		fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);

	Out.vShade = (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;

	vector		vLook = vPosition - g_vCamPosition;

	vector		vReflect = normalize(reflect(normalize(vLightDir), vNormal));

	Out.vSpecular = ((g_vLightSpecular * g_vMtrlSpecular) *
		pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 3.f)) * fAtt;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	//if (vDiffuse.a == 0.f)
	//	discard;

	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
	
    vector vRim = g_RimTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular + vRim;

	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
	
    //vector vShadow = g_ShadowTexture.Sample(LinearSampler, In.vTexcoord);
	
    //if (vDepthDesc.y  > vShadow.x)
    //    Out.vColor = vector(0, 0, 0, 1);
	
	
	float3 fogColor = float3(0.11,0.1,0.1);
	float fogMin = 0.0;
	float fogMax = 10.0;
	
	float dist = vDepthDesc.y*70.f;
	float distFog = saturate((dist-fogMin) / (fogMax - fogMin));


	float3 color = Out.vColor.xyz;
	color= lerp(color, fogColor, distFog);
	Out.vColor= float4(color,1.f);
	
    Out.vColor += float4(g_fFadeAlpha, g_fFadeAlpha, g_fFadeAlpha, 0);

	return Out;
}



technique11		DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Directional
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_RGBBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Point
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_RGBBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred_Blend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

    pass Post
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Sky, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }
}