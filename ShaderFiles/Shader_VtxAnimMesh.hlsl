
#include "Shader_Defines.hpp"


matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
/* 메시에게 영향을 주는 뼈행렬의 집합. */
matrix			g_BoneMatrices[256];
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

vector g_vCamPosition;
float g_fRimStrength;
float4 g_RimColor;
matrix g_ShadowViewMatrix;

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndices : BLENDINDEX;
	float4		vBlendWeights: BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;	
    float4		vShadowPos : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float	fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);


	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	/* 로컬상에서 뼈를 따라 움직인 정점의 위치가 된다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz;
	Out.vBinormal = cross(Out.vNormal, Out.vTangent);

	return Out;
}

VS_OUT VS_SHADOW(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);


    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	/* 로컬상에서 뼈를 따라 움직인 정점의 위치가 된다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ShadowViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = mul(vector(In.vTangent, 0.f), g_WorldMatrix).xyz;
    Out.vBinormal = cross(Out.vNormal, Out.vTangent);
    
    return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
    float4		vShadowPos : TEXCOORD3;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
    float4		vRim : SV_TARGET3;
    float4		vShadow : SV_TARGET4;
};

struct PS_SHADOW_OUT
{
    float4 vShadow : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector	vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	//vNormalDesc.b = sqrt(1 - saturate(dot(vNormalDesc.rg, vNormalDesc.rg)));

	if (vMtrlDiffuse.a <= 0.3f)
		discard;
	
	float3	vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vNormal = vector(vNormal * 0.5f + 0.5f,0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.f, 0.0f, 0.f);
	
    float3 toEye = normalize(g_vCamPosition - In.vWorldPos);
    float rim = (1.0 - dot(vNormal, toEye));
    Out.vRim = rim * g_RimColor * g_fRimStrength;
	
    //Out.vShadow = vector(In.vShadowPos.z / In.vShadowPos.w, In.vShadowPos.w / 300.f, 0.0f, 0.f);
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

technique11		DefaultTechnique
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