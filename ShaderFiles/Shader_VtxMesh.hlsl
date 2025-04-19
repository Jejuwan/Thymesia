
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_ShadowTexture;

vector g_vCamPosition;
float g_fRimStrength;
float4 g_RimColor;

float g_fAlpha;
float g_TexcoordV;

matrix g_ShadowViewMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz;
    Out.vBinormal = cross(Out.vNormal, Out.vTangent);
    return Out;
}

VS_OUT VS_SHADOW(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV3, matWVP3;

    matWV3 = mul(g_WorldMatrix, g_ShadowViewMatrix);
    matWVP3 = mul(matWV3, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP3);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz;
    Out.vBinormal = cross(Out.vNormal, Out.vTangent);
    
    //matrix matWV2 = mul(g_WorldMatrix, g_ShadowViewMatrix);
    //matrix matWVP2 = mul(matWV2, g_ProjMatrix);
    
    //Out.vShadowPos = mul(vector(In.vPosition, 1.f), matWVP2);
    
    return Out;
}

VS_OUT VS_EFFECT(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord.y += g_TexcoordV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz;
    Out.vBinormal = cross(Out.vNormal, Out.vTangent);
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;   
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vRim : SV_TARGET3;
    float4 vShadowFactor : SV_TARGET4;
};

struct PS_SHADOW_OUT
{
    float4 vShadow : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
      
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = normalize(mul(vNormal, WorldMatrix));

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
    
    	// Shadow map
    float shadowFactor = 1.f;

    //const float nearZ = 0.01; // 카메라 설정과 동일
        
    matrix lightViewProjMatrix = mul(g_ShadowViewMatrix, g_ProjMatrix);
    
        // 1. Project posWorld to light screen    
    float4 lightScreen = mul(In.vWorldPos, lightViewProjMatrix);
    lightScreen.xyz /= lightScreen.w;
        
        // 2. 카메라(광원)에서 볼 때의 텍스춰 좌표 계산
    float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
    lightTexcoord += 1.0;
    lightTexcoord *= 0.5;
        
        // 3. 쉐도우맵에서 값 가져오기
    float depth = g_ShadowTexture.Sample(LinearSampler, lightTexcoord).r;
        
        // 4. 가려져 있다면 그림자로 표시
    if (depth + 0.001 < lightScreen.z)
        shadowFactor = 0.f;
    
    Out.vShadowFactor = vector(shadowFactor, shadowFactor, shadowFactor, 1);
    return Out;
}

PS_SHADOW_OUT PS_SHADOW(PS_IN In)
{
    PS_SHADOW_OUT Out = (PS_SHADOW_OUT) 0;

    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //if (vMtrlDiffuse.a <= 0.3f)
    //    discard;
    
    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    //float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //vNormal = normalize(mul(vNormal, WorldMatrix));

    //Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
 
	
    Out.vShadow = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
    
    return Out;
}

PS_OUT PS_PLAGUE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;


    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = normalize(mul(vNormal, WorldMatrix));

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);

    float3 toEye = normalize(g_vCamPosition - In.vWorldPos);
    float rim = (1.0 - dot(vNormal, toEye));
    Out.vRim = rim * g_RimColor * g_fRimStrength;

 
    return Out;
}


PS_OUT PS_EFFECT_CLAW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	//vNormalDesc.b = sqrt(1 - saturate(dot(vNormalDesc.rg, vNormalDesc.rg)));

    //if (vMtrlDiffuse.a <= 0.3f)
    //    discard;
    
    vMtrlDiffuse.r = 0;
    vMtrlDiffuse.b = 0;
    vMtrlDiffuse.a = vMtrlDiffuse.g*g_fAlpha;

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	

    return Out;
}

PS_OUT PS_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    //vNormalDesc.b = sqrt(1 - saturate(dot(vNormalDesc.rg, vNormalDesc.rg)));

    //if (vMtrlDiffuse.a <= 0.3f)
    //    discard;

    vMtrlDiffuse.a = vMtrlDiffuse.g * g_fAlpha;

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);


    return Out;
}

technique11 DefaultTechnique
{
    pass Mesh
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

    pass EFFECT_CLAW
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_CLAW();
    }

    pass PLAGUE
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_PLAGUE();
    }

    pass EFFECT
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT();
    }

    pass SHADOW
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_SHADOW();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOW();
    }
}