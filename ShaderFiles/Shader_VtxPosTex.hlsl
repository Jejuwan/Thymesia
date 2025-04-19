
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_DepthTexture;

int g_iFrame;
int g_iRow;
float2 textureOffset = float2(0.0, 0.0); // 시작 위치 (0.25, 0.25)로 설정
float2 textureScale; // 크기를 반으로 줄임
float g_fAlpha;
float g_fHp;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

/* SV_POSITION에 해당하는 데이터의 W값으로 XYZW를 나눈다.(투영변환) */
/* 뷰포트 스페이스로 변환한다. (윈도우좌표로 변환한다.) */
/* 래스터라이즈를 수행한다. (픽셀을 생성한다.) */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	/* 0번째 렌더타겟에 그리는 픽셀의 색상이다. */
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_fAlpha < 0.95f)
        Out.vColor.a = g_fAlpha;

    if (In.vTexcoord.x > g_fHp)
        discard;

    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vUV;

    vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vUV);

	/* 이펙트 픽셀을 그릴려고 하는 위치에 이미 그려져 있었던 픽셀의 깊이. */
    float fViewZ = vDepthDesc.y * 300.f;

    float fAlpha = saturate((fViewZ - In.vProjPos.w) * 2.f);

    int sum = g_iFrame / g_iRow;
    int nam = g_iFrame % g_iRow;
    float2 size = float2((float) 1 / g_iRow, (float) 1 / g_iRow);
    
    float2 adjustedTexCoord = (In.vTexcoord + float2(nam - 1, sum - 1)) * size;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, adjustedTexCoord);

    Out.vColor = vector(vColor.xyz, vColor.a * fAlpha);   
    
    return Out;
}

struct PS_IN_SPARK
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

PS_OUT PS_MAIN_SPARK(PS_IN_SPARK In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vUV;

    vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vUV);

	/* 이펙트 픽셀을 그릴려고 하는 위치에 이미 그려져 있었던 픽셀의 깊이. */
    float fViewZ = vDepthDesc.y * 300.f;

    float fAlpha = saturate((fViewZ - In.vProjPos.w) * 2.f);

    int sum = g_iFrame / g_iRow;
    int nam = g_iFrame % g_iRow;
    float2 size = float2((float) 1 / g_iRow, (float) 1 / g_iRow);
    
    
    float2 adjustedTexCoord = (In.vTexcoord + float2(nam - 1, sum - 1)) * size;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, adjustedTexCoord);
    
    vColor.a = vColor.r;
    
    vColor.b = 0;

    Out.vColor = vector(vColor.xyz, vColor.a * fAlpha * g_fAlpha);

    return Out;
}

PS_OUT PS_MAIN_EFFECT_GREEN(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV;

    vUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vUV);

    /* 이펙트 픽셀을 그릴려고 하는 위치에 이미 그려져 있었던 픽셀의 깊이. */
    float fViewZ = vDepthDesc.y * 300.f;

    float fAlpha = saturate((fViewZ - In.vProjPos.w) * 2.f);

    int sum = g_iFrame / g_iRow;
    int nam = g_iFrame % g_iRow;
    float2 size = float2((float)1 / g_iRow, (float)1 / g_iRow);

    float2 adjustedTexCoord = (In.vTexcoord + float2(nam - 1, sum - 1)) * size;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, adjustedTexCoord);
    vColor.rb = 0;

    Out.vColor = vector(vColor.xyz, vColor.a * fAlpha);

    return Out;
}



technique11 DefaultTechnique
{
    pass UI
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

    pass EFFECT
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass SPARK
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPARK();
    }

    pass EFFECT_GREEN
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT_GREEN();
    }
}