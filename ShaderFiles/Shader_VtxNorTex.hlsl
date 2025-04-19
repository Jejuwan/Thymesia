
#include "Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vBrushPos = vector(5.f, 0.0f, 5.f, 1.f);
float g_fRadius = 3.f;

texture2D g_DiffuseTexture[2];
texture2D g_MaskTexture;
texture2D g_BrushTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vBrush = /*g_BrushTexture.Sample(LinearSampler, In.vTexcoord)*/vector(0.f, 0.f, 0.f, 0.f);

    if (g_vBrushPos.x - g_fRadius < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRadius &&
		g_vBrushPos.z - g_fRadius < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRadius)
    {
        float2 vTexcoord;

        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRadius)) / (2.f * g_fRadius);
        vTexcoord.y = ((g_vBrushPos.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius);

        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }


    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);

    vector vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask) + vBrush;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = 1.f;

	/*In.vNormal 각 성분이 -1 ~ 1 */
	/*Out.vNormal 각 성분이 0 ~ 1 */
    Out.vNormal = In.vNormal * 0.5f + 0.5f;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass Terrain
    {
        SetRasterizerState( /*RS_Wireframe*/RS_Default);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	/*pass Delete
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DELETE();
	}*/
}